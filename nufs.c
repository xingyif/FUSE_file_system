#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <bsd/string.h>
#include <assert.h>

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <fcntl.h>

#include "storage.h"
#include "superblock.h"
#include "util.h"
#include "iblock.h"
#include "inode.h"
#include "slist.h"
#include "directory.h"

// implementation for: man 2 access
// Checks if a file exists.
int
nufs_access(const char *path, int mask) {
    // todo doesn't differentiate owner, group, other, if not == user, return error
    printf("access(%s, %04o)\n", path, mask); // debugging purpose
    // checks if the path exists
    int index = get_entry_index(path);
    if (index < 0) {
        return -ENOENT; // path doesn't exist
    }

    // get current user id
    int cur_uid = getuid();
    inode *cur_inode = single_inode_addr(index);
    // todo not checking permission for getting rid of the error
    /* current user is not the owner
    if (cur_inode->user_id != cur_uid) {
        return -EACCES;
    } // todo need to fix how these are being set for step 2 and supporting metadeta
    // when cur_user == file owner
    if (cur_inode->mode != mask) {
        return -EACCES;
    }*/

    // Read, write, execute/search by owner

    // check u_id? return -EACCESS if the requested permission isn't available
//    struct stat* st;
//    int rv = nufs_getattr(path, st);
//    assert(rv == 0);
    return 0; // success
}

// implementation for: man 2 stat
// gets an object's attributes (type, permissions, size, etc)
int
nufs_getattr(const char *path, struct stat *st) {

    printf("In nufs_getattr(%s)\n", path); // debugging purpose
    // get_stat will check if file/dir exist
    int rv = get_stat(path, st);
    if (rv == -1) {
        printf("in nufs_getattr, given path doesn't exist\n");
        return -ENOENT; // path doesn't exist
    }
    return rv;
}

// implementation for: man 2 readdir
// lists the contents of a directory
int
nufs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
             off_t offset, struct fuse_file_info *fi) {
    struct stat st;
    int index = get_entry_index(path);
    if (index < 0) {
        return -ENOENT; // path doesn't exist
    }
    // todo why do you create new inode and dir???????????????????????????????
//    inode *cur_inode = inodes_addr()[index];
//    directory *cur_dir = iblocks_addr()[index];

    inode *cur_inode = single_inode_addr(index);
    directory *cur_dir = single_iblock_addr(index);

    for (int i = 0; i < 32; i++) {
        dir_ent *cur_ent = cur_dir->entries[i];
        offset += (i * sizeof(dir_ent));
        if (cur_ent == NULL) {
            continue;
        } else if (filler(buf, cur_ent->filename, &st, offset) != 0) {

            return 0;
        }
    }
    printf("readdir(%s)\n", path);



    //   get_stat("/", &st);
    // filler is a callback that adds one item to the result
    // it will return non-zero when the buffer is full
    // filler(buf, ".", &st, 0);

//   get_stat("/hello.txt", &st);
    // filler(buf, "hello.txt", &st, 0);
    printf("made a hello.txt file\n");
    return 0;
}

// mknod makes a filesystem object like a file or directory
// called for: man 2 open, man 2 link
int
nufs_mknod(const char *path, mode_t mode, dev_t rdev) {
    mode = 010064;
    printf("in nufs_mknod:(%s, %04o)\n", path, mode);
    // checks if the path exists
    int index = get_entry_index(path);
    if (index >= 0) {
        return -EEXIST; // path already exist
    }

    int aval_idx = inode_bitmap_find_next_empty(inode_bitmap_addr());
    if (aval_idx < 0) {
        // ENOSPC: operation failed due to lack of disk space
        return aval_idx;
    }

    // create the new inode ptr
    inode *cur_inode = single_inode_addr(aval_idx);
//    inodes_addr()[aval_idx] = cur_inode;
    inode_init(cur_inode, mode, 1, 0);
    // flush the inode ptr to disk
    inodes_addr()[aval_idx] = cur_inode;
    // put the initialized inode to inodes
    // update inode_bitmap
    inode_bitmap_addr()[aval_idx] = 1;

    // create the new iblock ptr
    iblock *cur_iblock = single_iblock_addr(aval_idx);
    // flush the iblock ptr to disk
    iblocks_addr()[aval_idx] = cur_iblock;
    // update the iblock_bitmap
    iblock_bitmap_addr()[aval_idx] = 1;

    // create an entry and add the new entry to its own home dir
    int new_entry_idx = add_dir_entry(path, aval_idx);
    if (new_entry_idx < 0) {
        // ERROR out
        return new_entry_idx;
    }

    printf("after mknod(%s, %04o)\n", path, mode);
    return 0; // success
}

// most of the following callbacks implement
// another system call; see section 2 of the manual
int
nufs_mkdir(const char *path, mode_t mode) {
    printf("in nufs_mkdir:(%s, %04o)\n", path, mode);

    // checks if the path exists
    int index = get_entry_index(path);
    if (index >= 0) {
        //     return -EEXIST; // path already exists
    }

    int aval_idx = inode_bitmap_find_next_empty(inode_bitmap_addr());
    if (aval_idx < 0) {
        return aval_idx; // ENOSPC: operation failed due to lack of memory or disk space
    }
    // create the new inode ptr
    inode *cur_inode = single_inode_addr(aval_idx);
//    inodes_addr()[aval_idx] = cur_inode;
    inode_init(cur_inode, mode, 1, 0);
    // flush the inode ptr to disk
    inodes_addr()[aval_idx] = cur_inode;
    // update inode_bitmap
    inode_bitmap_addr()[aval_idx] = 1;

    // create the new iblock ptr
    directory *cur_dir = single_iblock_addr(aval_idx);

    // create the new directory ptr
    directory_init(cur_dir, slist_last(path)->data);
    // flush the dir ptr to disk
    iblocks_addr()[aval_idx] = cur_dir;
    // update the iblock_bitmap
    iblock_bitmap_addr()[aval_idx] = 1;

    // check iblock, if entry does not exist, then create it and put it to the given dir
    int new_entry_idx = add_dir_entry(path, aval_idx);
    if (new_entry_idx < 0) {
        // ERROR out
        return new_entry_idx;
    }

    printf("after mkdir(%s)\n", path);
    return -1;
}

// remove a file. Returns success, ENOENT, or EISDIR
int
nufs_unlink(const char *path) {
    printf("unlink(%s)\n", path);
    // checks if the path exists
    int index = get_entry_index(path);
    if (index < 0) {
        return -ENOENT; // path doesn't exist
    }

    inode* cur_inode = inodes_addr()[index]; // todo change to single_addr.. ?
    // given path is a dir not a file
    if (!cur_inode->is_file) {
        return -EISDIR;
    }

    // delete the entry from its home dir
    int rv = remove_dir_entry(path);

    // set cur_inode & cur_iblock to be null
    iblocks_addr()[index] = NULL;
    inodes_addr()[index] = NULL;

    return rv; // success or failure
}

int
nufs_rmdir(const char *path) {
    printf("rmdir(%s)\n", path);
    // checks if the path exists
    int index = get_entry_index(path);
    if (index < 0) {
        return -1; // ENOENT: path doesn't exist
    }

    // remove inode, and remove iblock_ptrs at the given index
    inodes_addr()[index] = NULL;
    iblocks_addr()[index] = NULL;
    // update the bitmaps
    inode_bitmap_addr()[index] = 0;
    iblock_bitmap_addr()[index] = 0;

    return 0; // success
}

// implements: man 2 rename
// called to move a file within the same filesystem
int
nufs_rename(const char *from, const char *to) {
    // todo should i call access
    printf("rename(%s => %s)\n", from, to);
    // checks if the path exists
    int from_index = get_entry_index(from);
    if (from_index < 0) {
        return -1; // ENOENT: path doesn't exist
    }

    int to_index = get_entry_index(to);
    // todo??? it is safe to inlcude 0=> root
    // if to exists, then remove it first
    if (to_index >= 0) {

    }



    // if two names are the same, do nothing
    if (strcmp(from, to) == 0) {
        return 0;
    }

    inode *from_inode = inodes_addr()[from_index];
    iblock *from_iblock = iblocks_addr()[from_index];

    // create inode & iblock for to
    inode *to_inode = single_inode_addr(to_index);


    // both from and to must be the same type either file/dir


//    EISDIR // new is a directory, but old is not a directory.
//
//    ENOTDIR // old is a directory, but new is not a directory.
//    // if to exists, then it is removed/replaced
//



    return -1;
}

int
nufs_chmod(const char *path, mode_t mode) {
    printf("chmod(%s, %04o)\n", path, mode);
    return -1;
}

int
nufs_truncate(const char *path, off_t size) {
    printf("truncate(%s, %ld bytes)\n", path, size);
    return -1;
}

// this is called on open, but doesn't need to do much
// since FUSE doesn't assume you maintain state for
// open files.
int
nufs_open(const char *path, struct fuse_file_info *fi) {
    printf("open(%s)\n", path);

    // checks if the path exists
    int from_index = get_entry_index(path);
    if (from_index < 0) {
        return -ENOENT; // path doesn't exist
    }
    // todo do i need to check for read only???????????????????????????????????????????????????????????
    // checks if it is read only
    if ((fi->flags & 3) != O_RDONLY) {
        return -EACCES; // no access to file
    }

    return 0;
}

// Actually read data
int
nufs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    printf("read(%s, %ld bytes, @%ld)\n", path, size, offset);

    // checks if the path exists
    int index = get_entry_index(path);
    if (index < 0) {//what if we are creating a new text??
        return -1; // ENOENT: path doesn't exist
    }

    // data can be file contents or directory contents
    void *data = get_data(path);

    inode *cur_inode = single_inode_addr(index);
    if (offset > cur_inode->size_of) {
        return 0;
    }
    /*
    int len = strlen(data) + 1;
    if (size < len) {
        len = size;
    }*/

    char *new_blk;
    for (int position = offset; position < offset + size;) {
        memmove(buf, new_blk + position % 4096, 4096 - position % 4096);
    }
    return size;
}

// Actually write data
int
nufs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    printf("write(%s, %ld bytes, @%ld)\n", path, size, offset);
// get iblock index for this path
    int index = get_entry_index(path);
// get block with the index
    iblock *cur_block = single_iblock_addr(index);
    if (offset + size > 4096) {
        return -ENOENT;
    }
    char *new_blk;
    for (int position = offset; position < offset + size;) {
        memmove(new_blk + position % 4096, buf, 4096 - position % 4096);
    }
    return size;
}

void
nufs_init_ops(struct fuse_operations *ops) {
    memset(ops, 0, sizeof(struct fuse_operations));
    ops->access = nufs_access;
    ops->getattr = nufs_getattr;
    ops->readdir = nufs_readdir;
    ops->mknod = nufs_mknod;
    ops->mkdir = nufs_mkdir;
    ops->unlink = nufs_unlink;
    ops->rmdir = nufs_rmdir;
    ops->rename = nufs_rename;
    ops->chmod = nufs_chmod;
    ops->truncate = nufs_truncate;
    ops->open = nufs_open;
    ops->read = nufs_read;
    ops->write = nufs_write;
};

struct fuse_operations nufs_ops;

int
main(int argc, char *argv[]) {
    assert(argc > 2);
//	slist* path = split(argv[--argc]); - also call util function 
    storage_init(argv[--argc]); // disk_image
//   superblock_add_inode(argv[--argc]);
    nufs_init_ops(&nufs_ops);
    // mount happened magically in fuse_main
    return fuse_main(argc, argv, &nufs_ops, NULL);
}

