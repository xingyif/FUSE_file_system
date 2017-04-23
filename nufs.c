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
    printf("access(%s, %04o)\n", path, mask);
    // checks if the path exists
    int index = get_entry_index(path);
    if (index < 0) {
        return -ENOENT; // path doesn't exist
    }

    // get current user id
    int cur_uid = getuid();
    inode *cur_inode = single_inode_addr(index);

    return 0; // success
}

// implementation for: man 2 stat
// gets an object's attributes (type, permissions, size, etc)
int
nufs_getattr(const char *path, struct stat *st) {

    printf("In nufs_getattr(%s)\n", path);
    // get_stat will check if file/dir exist
    int rv = get_stat(path, st);
    printf("get_attr(%s) -> (%d) {mode: %04o, size: %d}\n", path, rv, st->st_mode, st->st_size);
    return rv;
}

// implementation for: man 2 readdir
// lists the contents of a directory
int
nufs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
             off_t offset, struct fuse_file_info *fi) {
    printf("in nufs_readdir path: %s buf:%s offset: %d\n", path, buf, offset);

    int index = get_entry_index(path);
    if (index < 0) {
        return -ENOENT; // path doesn't exist
    }
    directory *cur_dir = single_iblock_addr(index);
    for (int i = 0; i < cur_dir->number_of_entries; i++) {
        dir_ent cur_ent = cur_dir->entries[i];

        //offset += (i * sizeof(dir_ent));
        // if cur_ent == NULL
        if (cur_ent.filename == NULL) {
            continue;
        } else {
            filler(buf, cur_ent.filename, NULL, 0);
            continue;
        }
    }
    return 0;
}

// mknod makes a filesystem object like a file or directory
// called for: man 2 open, man 2 link
int
nufs_mknod(const char *path, mode_t mode, dev_t rdev) {
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

    // find the new inode ptr
    inode *cur_inode = single_inode_addr(aval_idx);
    // initialized inode and the inode struct is stored in disk
    inode_init(cur_inode, mode, 1, 0);
    // update inode_bitmap
    inode_bitmap_addr()[aval_idx] = 1;

    // find the new iblock ptr
    // update the iblock_bitmap
    iblock_bitmap_addr()[aval_idx] = 1;

    // create an entry and add the new entry to its own home dir
    int new_entry_idx = add_dir_entry(path, aval_idx);
    if (new_entry_idx < 0) {
        // ERROR out
        return new_entry_idx;
    }

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
        return -EEXIST; // path already exists
    }

    int aval_idx = inode_bitmap_find_next_empty(inode_bitmap_addr());
    if (aval_idx < 0) {
        return aval_idx; // ENOSPC: operation failed due to lack of memory or disk space
    }
    // create the new inode ptr
    inode *cur_inode = single_inode_addr(aval_idx);
    inode_init(cur_inode, mode, 1, 0);
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

    inode *cur_inode = single_inode_addr(index);
    // given path is a dir not a file
    if (!cur_inode->is_file) {
        return -EISDIR;
    }

    // delete the entry from its home dir
    int rv = remove_dir_entry(path);

    // set the bitmaps to be avaliable
    inode_bitmap_addr()[index] = 0;
    iblock_bitmap_addr()[index] = 0;

    return rv; // success or failure
}

int
nufs_rmdir(const char *path) {
    printf("rmdir(%s)\n", path);
    // checks if the path exists
    int index = get_entry_index(path);
    if (index < 0) {
        return index; // ENOENT: path doesn't exist
    }

    inode *cur_inode = single_inode_addr(index);
    // given path is a dir not a file
    if (cur_inode->is_file) {
        return -ENOTDIR; // given path is not dir
    }

    int rv = remove_dir_entry(path);
    // update the bitmaps
    inode_bitmap_addr()[index] = 0;
    iblock_bitmap_addr()[index] = 0;

    return rv; // success or failure
}

// implements: man 2 rename
// called to move a file within the same filesystem
int
nufs_rename(const char *from, const char *to) {
    printf("rename(%s => %s)\n", from, to);
    // checks if the path exists
    int from_index = get_entry_index(from);
    if (from_index < 0) {
        return -1; // ENOENT: path doesn't exist
    }

    int to_index = get_entry_index(to);
    // if to exists, then remove it first
    if (to_index >= 0) {

    }

    // if two names are the same, do nothing
    if (strcmp(from, to) == 0) {
        return 0;
    }

    inode *from_inode = single_inode_addr(from_index);
    iblock *from_iblock = single_iblock_addr(from_index);

    // create inode & iblock for to
    inode *to_inode = single_inode_addr(to_index);
    // both from and to must be the same type either file/dir
//    EISDIR // new is a directory, but old is not a directory.
//    ENOTDIR // old is a directory, but new is not a directory
    return -1;
}

int
nufs_chmod(const char *path, mode_t mode) {
    printf("chmod(%s, %04o)\n", path, mode);
    int index = get_entry_index(path);
    inode *from_inode = single_inode_addr(index);
    iblock *from_iblock = single_iblock_addr(index);
    return -1;
}

int
nufs_truncate(const char *path, off_t size) {
    printf("truncate(%s, %ld bytes)\n", path, size);
    int index = get_entry_index(path);
    inode *from_inode = single_inode_addr(index);
    iblock *from_iblock = single_iblock_addr(index);
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
    return 0;
}

// Actually read data
int
nufs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    printf("read(%s, %ld bytes, @%ld)\n", path, size, offset);

    // checks if the path exists
    int index = get_entry_index(path);
    if (index < 0) {//what if we are creating a new text??
        return -ENOENT; // ENOENT: path doesn't exist
    }

    // data can be file contents or directory contents
    iblock *cur_iblock = single_iblock_addr(index);
    if (offset + size > 4096) {
        return -ENOENT;
    }
    size = size - offset;
    memcpy(buf, cur_iblock + offset, size);
    return strlen(buf);
}

// Actually write data
int
nufs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    printf("write(%s, %d bytes, @%d, %s buf)\n", path, size, offset, buf);
    // get iblock index for this path
    int index = get_entry_index(path);
    if (index < 0) {
        return -EISDIR;
    }
    // get block with the index
    iblock *cur_block = single_iblock_addr(index);
    if (offset + size > 4096) {
        return -ENOENT;
    }
    memcpy(cur_block + offset, buf, size);
    printf("in write afte memcpy at cur_block: %s\n", cur_block);
    inode *cur_inode = single_inode_addr(index);
    cur_inode->size_of = size;
    return size;
}

int
nufs_destroy() {
    storage_free();
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
    ops->fusermount = nufs_destroy;
};

struct fuse_operations nufs_ops;

int
main(int argc, char *argv[]) {
    assert(argc > 2);
    storage_init(argv[--argc]); // disk_image
    nufs_init_ops(&nufs_ops);
    // mount happened magically in fuse_main
    return fuse_main(argc, argv, &nufs_ops, NULL);
}

