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

const int DISK_SIZE = 1024 * 1024; // 1MB
void *disk;

static int   pages_fd   = -1;
static void* pages_base = 0;

void
storage_init(char *disk_image) {

    pages_fd = open(disk_image, O_CREAT | O_RDWR, 0644);
    assert(pages_fd != -1);

    int rv = ftruncate(pages_fd, DISK_SIZE);
    assert(rv == 0);

    pages_base = mmap(0, DISK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, pages_fd, 0);
    assert(pages_base != MAP_FAILED);
    superblock_init(disk);

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


    // update inode bitmap
    inode_bitmap_addr()[root_dir_idx] = 1;

    //creating iblock root_dir here
    directory *root_iblock = single_iblock_addr(root_dir_idx);
    // setting up root_dir block
    char *root_dir_name = "/";
    // get dir* from iblocks and initialize the root_dir
    directory_init(root_iblock, root_dir_name);

    // update iblock bitmap
    iblock_bitmap_addr()[root_dir_idx] = 1;
}

int
get_entry_index(char *path) {
    // 1. truncate path
    // 2. get inodes
    // 3. get iblocks

    int current_inode_idx = superblock_addr()->root_inode_idx;
    directory *root_dir = single_iblock_addr(current_inode_idx);
    if (streq(path, root_dir->dir_name)) {
        return 0;
    }
    slist *path_list = s_split(path, '/');

    // get to the name we are looking for
    if (streq(path_list->data, "")) {
        path_list = path_list->next;
    }
    directory *current = root_dir;
    while (path_list != NULL) {
        // get the index of the entry
        int entry_idx = directory_entry_lookup(current, path_list->data);
        // didn't find the entry
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
            // put the entry array in
            int new_entry_idx = directory_insert_entry(current, path_list->data, new_inode_idx);
            return new_entry_idx; // success or didn't successfully insert
        } else {
            // if didn't find it, and the next is not null, throw path doesn't exist exception
            if (entry_idx < 0) {
                return -ENOENT;
            }

            // haven't finished yet, keep traversing
            dir_ent cur_ent = current->entries[entry_idx];
            int entry_inode_index = cur_ent.entry_inode_index;
            current = single_iblock_addr(entry_inode_index); // iblocks_addr()[entry_inode_index];
            path_list = path_list->next;
        }
    }
    return -EEXIST;
}


int
remove_dir_entry(char *path) {
    slist *path_list = s_split(path, '/');
    directory *root_dir = single_iblock_addr(
            superblock_addr()->root_inode_idx);

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
    int index = get_entry_index(path);
    if (index < 0) {
        // didn't find the given path
        return -ENOENT;
    }
    // write sizeof(stat) bytes of 0 to st
    memset(st, 0, sizeof(struct stat));

    inode *cur_inode = single_inode_addr(index); // inodes_addr()[index];
    st->st_uid = cur_inode->user_id;
    st->st_mode = cur_inode->mode;

    st->st_size = cur_inode->size_of;
    return 0;
}

void *
get_data(char *path)
{
    // assuming that the given path is to a file not a directory
    int index = get_entry_index(path);
    if (index < 0) {
        return -ENOENT;
    }
    inode *cur_inode = single_inode_addr(index);
    // if we are looking at a file
    if (cur_inode->is_file) {
        iblock *cur_iblock = single_iblock_addr(index);
        return cur_iblock->contents;
    }
    // if we are looking at a directory
    directory *cur_dir = single_iblock_addr(index);
    return cur_dir;
}


void *
get_disk() {
    return disk;
}

void
storage_free()
{
    int rv = munmap(pages_base, DISK_SIZE);
    assert(rv == 0);
}
