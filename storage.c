// ATTENTION: this should be where everything {superblock, bitmaps, inodes, iblocks} exist

// this file should be the middleware => operation file
// created by Nat Tuck
// mofied by Yifan & Brett
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

#include "directory.h"
#include "superblock.h"
#include "inode.h"
#include "util.h"
#include "iblock.h"
#include "storage.h"
#include "pages.h"
#include "slist.h"

typedef struct file_data {
    const char *path;
    int mode;
    const char *data;
} file_data;
const int DISK_SIZE = 1024 * 1024; // 1MB
void *disk;

static int   pages_fd   = -1;
static void* pages_base =  0;

void
storage_init(char *disk_image) {

    printf("in storage_init, given disk_image(%s)\n", disk_image);
    pages_fd = open(disk_image, O_CREAT | O_RDWR, 0644);
    assert(pages_fd != -1);

    int rv = ftruncate(pages_fd, DISK_SIZE);
    assert(rv == 0);

    pages_base = mmap(0, DISK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, pages_fd, 0);
    assert(pages_base != MAP_FAILED);

//    int fd;
//    if ((fd = open(disk_image, O_CREAT | O_RDWR, 0644)) == -1) {
//        perror("Opening disk image failed!");
//        exit(1);
//    }
//    perror("? ");
//
//    int rv = ftruncate(fd, DISK_SIZE);
//    assert(rv == 0);
//
//    // returns a non-negative integer, termed a file descriptor.  It returns -1 on failure, and sets
//    // errno to indicate the error return - value if failed
//    // initialize superblock if it has never been initialized before
//    disk = mmap(NULL, DISK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
//    if (disk == MAP_FAILED) {
//        perror("Couldn't map image");
//        exit(1);
//    }
    printf("in storage_init, mmaped the disk\n");
    superblock_init(disk);


    // bitmaps initilized, fixed sized in storage.h
    // inodes initilized, fixed sized in storage.h
    // iblocks initilized, fixed sized in storage.h
    // create the root dir and put it to inodes and iblocks
    printf("in storage_init, after mmap disk, disk_ptr: %p\n", disk);

    // setting up inode_bitmap and iblock_bitmap
    for (int i = 0; i < 256; i++) {
        inode_bitmap_addr()[i] = 0;
        iblock_bitmap_addr()[i] = 0;
    }

    // setting up root_dir inode
    superblock *sprblk_addr = superblock_addr();
    int root_dir_idx = sprblk_addr->root_inode_idx;
    // get inode* from inodes
    inode *root_inode = single_inode_addr(root_dir_idx);

    inode_init(root_inode, 040755, 0, 4096); // S_IRWXU | S_IRWXG | S_IRWXO

    printf("in storage_init, calculated root inode ptr(%p)\n", root_inode);

    // update inode bitmap
    inode_bitmap_addr()[root_dir_idx] = 1;

    //creating iblock root_dir here
    directory *root_iblock = single_iblock_addr(root_dir_idx);

    // setting up root_dir block
    char *root_dir_name = "/";
    // get dir* from iblocks and initialize the root_dir
    directory_init(root_iblock, root_dir_name);
    iblock_bitmap_addr()[root_dir_idx] = 1;
}

int
get_entry_index(char *path) {
    // 1. truncate path
    // 2. get inodes
    // 3. get iblocks
    printf("in get_entry_index, given path is(%s)\n", path);

    int current_inode_idx = superblock_addr()->root_inode_idx;
    directory *root_dir = single_iblock_addr(current_inode_idx);
    if (streq(path, root_dir->dir_name)) {
        printf("in get_entry_index, path(%s) is home dir, return 0;\n", path);
        return 0;
    }
    slist *path_list = s_split(path, '/');
    printf("in get_entry_index, given path is not home dir, first path(%s)\n", path_list->data);
    printf("in get_entry_index, root_dir->dir_name(%s)\n", root_dir->dir_name);

    //  char* path_array = slist_close(path_list); don't need to use  slist_close returns a pointer to the array
    //todo check if user path starts at home else look at cur_dir path from home
    // fixme addr() returns ** because can't case void to directory
    //todo assuming that user is giving path that either starts with home dir or entry in home dir
    // get to the name we are looking for
    if (streq(path_list->data, "")) {
        path_list = path_list->next;
    }
    directory *current = root_dir;
    while (path_list != NULL) {
        printf("in get_entry_index, entered while loop, current path_list->data(%s)\n", path_list->data);
        // get the index of the entry
        int entry_idx = directory_entry_lookup(current, path_list->data);
        // didn't find the entry
        printf("in get_entry_index, in while loop, entry_idx(%d)\n", entry_idx);
        if (entry_idx < 0) {
            return -ENOENT; // no such file or dir
        }
        // get current entry from current dir
        dir_ent cur_ent = current->entries[entry_idx];
        int entry_inode_index = cur_ent.entry_inode_index;

        // find the file
        if (path_list->next == NULL) {
            return cur_ent.entry_inode_index;
        } else {
            // current is not a file
            current = single_iblock_addr(entry_inode_index); // iblocks_addr()[entry_inode_index];
            path_list = path_list->next;
        }
    }
    return -ENOENT;
}

int
add_dir_entry(char *path, int new_inode_idx) {
    printf("in add_dir_entry, given path(%s), new_inode_index(%d)\n", path, new_inode_idx);
    slist *path_list = s_split(path, '/');
    directory *root_dir = single_iblock_addr(
            superblock_addr()->root_inode_idx);

    // if in root dir, move path_list to the next
    if (streq(path_list->data, "")) {
        path_list = path_list->next;
    }

    directory *current = root_dir;
    while (path_list != NULL) {
        // get the index of the entry
        int entry_idx = directory_entry_lookup(current, path_list->data);
        // didn't find the entry, and the path_list is the last in the list
        // make a new entry and put it in cur_dir
        if ((entry_idx < 0) && (path_list->next == NULL)) {
            printf("in add_dir_entry, maked a new entry, and insert it to the cur_dir, "
                           "current_dir_name(%s), new name(%s), new_inode_idx(%d)\n", current->dir_name, path_list->data, new_inode_idx);
            // put the entry array in
            int new_entry_idx = directory_insert_entry(current, path_list->data, new_inode_idx);
            return new_entry_idx; // success or didn't successfully insert
        } else {
            // if didn't find it, and the next is not null, throw path doesn't exist exception
            if (entry_idx < 0) {
                printf("in add_dir_entry, didn't find dir(%s), about to return ENOENT", current->dir_name);
                return -ENOENT;
            }

            printf("in add_dir_entry, having find the destination dir/file yet, keep searching\n");
            // haven't finished yet, keep traversing
            dir_ent cur_ent = current->entries[entry_idx];
            int entry_inode_index = cur_ent.entry_inode_index;
            current = single_iblock_addr(entry_inode_index); // iblocks_addr()[entry_inode_index];
            path_list = path_list->next;
        }
    }
    printf("in add_dir_entry, path_list is = NULL, didn't enter while loop, about to return ENOENT, given path(%s)\n", path);
    return -EEXIST;
}


int
remove_dir_entry(char *path) {
    slist *path_list = s_split(path, '/');
    directory *root_dir = single_iblock_addr(
            superblock_addr()->root_inode_idx); // (directory *) (iblocks_addr()[superblock_addr()->root_inode_idx]);

    // if in root dir, move path_list to the next
    if (streq(path_list->data, root_dir->dir_name)) {
        path_list = path_list->next;
    }

    directory *current = root_dir;
    while (path_list != NULL) {
        // get the index of the entry
        int entry_idx = directory_entry_lookup(current, path_list->data);
        // didn't find current entry
        // return path not found error
        if (entry_idx < 0) {
            return -ENOENT;
        }
        // found it, and the path_list is the last in the list, delete it
        if (path_list->next == NULL) {
            int rv = directory_del_entry(current, entry_idx);
            return rv; // either failure or success
        } else {
            // haven't finished yet, keep traversing
            dir_ent cur_ent = current->entries[entry_idx];
            int entry_inode_index = cur_ent.entry_inode_index;
            current = single_iblock_addr(entry_inode_index); // iblocks_addr()[entry_inode_index];
            path_list = path_list->next;
        }
    }
    return -ENOENT;
}

int
get_stat(char *path, struct stat *st) {
    printf("in get_stat, given path(%s)\n", path);
    int index = get_entry_index(path);
    if (index < 0) {
        // didn't find the given path
        printf("in get_stat, given path doesn't exist, about to return ENOENT\n");
        return -ENOENT;
    }
    // write sizeof(stat) bytes of 0 to st
    memset(st, 0, sizeof(struct stat));

    inode *cur_inode = single_inode_addr(index); // inodes_addr()[index];
    st->st_uid = cur_inode->user_id;
    st->st_mode = cur_inode->mode;

    st->st_size = cur_inode->size_of;
    printf("in get_stat, successfully filled out the st, return 0\n");
    return 0;
}

void *
get_data(char *path) // todo do we always assume the path is a file???????????? no
{
    printf("in get_data, given path(%s)\n", path);
    // assuming that the given path is to a file not a directory
    int index = get_entry_index(path);
    if (index < 0) {
        printf("in get_data, given path doesn't exist, about to return ENOENT\n");
        return -ENOENT;
    }
    inode *cur_inode = single_inode_addr(index); // inodes_addr()[index];
    // if we are looking at a file
    if (cur_inode->is_file) {
        iblock *cur_iblock = single_iblock_addr(index); // iblocks_addr()[index];
        printf("in get_data, found the given path to a file, about to return file content\n");
        return cur_iblock->contents;
    }
    // if we are looking at a directory
    directory *cur_dir = single_iblock_addr(index); // iblocks_addr()[index];
    printf("in get_data, found the given path to a dir, about to return cur_dir_ptr(%p)\n", cur_dir);
    return cur_dir;
}


void *
get_disk() {
    return disk;
}
