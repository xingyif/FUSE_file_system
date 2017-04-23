#include "inode.h"
#include "storage.h"
#include <sys/stat.h>
#include <sys/mman.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "superblock.h"
void
inode_init(inode* cur_inode, mode_t mode, int is_file, size_t size) {
    // good default mode
    // Directory: 040755
    // Regular file: 0100644

    // todo, should inode contain file name
    cur_inode->user_id = getuid();
    cur_inode->mode = mode;
    cur_inode->is_file = is_file;
    cur_inode->size_of = size;
}

int
inode_bitmap_find_next_empty(int* inode_bitmap_ptr)
{
    int inode_index = -ENOSPC; // operation failed due to lack of disk space
    for (int ii = 1; ii < 256; ii++) {
        if (inode_bitmap_ptr[ii] == 0) { // if iblock is empty
            inode_index = ii;
            break;
        }
    }
    // return value >= 0 if success, else return -ENOMEM for failure
    return inode_index;
}

inode*
inodes_addr() {
    return (inode*) (get_disk() + superblock_addr()->inodes);
}

int*
inode_bitmap_addr() {
    return (int*) (get_disk() + superblock_addr()->ibitmap_location);
}

void*
single_inode_addr(int idx) {
    return (void*) (inodes_addr() + sizeof(inode) * idx);
}
