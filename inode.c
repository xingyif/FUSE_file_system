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
//    inode_fd = open(path, O_CREAT | O_RDWR, 0644);
//    assert(inode_fd != -1);

    // the regular file named by path or referenced by fd to be truncated to a size of precisely length bytes.
//    int rv = ftruncate(iblock_fd, NUFS_SIZE);
//    assert(rv == 0); // success

    // good default mode
    // Directory: 040755
    // Regular file: 0100644

    // todo, should inode contain file name
    cur_inode->user_id = getuid();
    cur_inode->mode = mode;
    cur_inode->is_file = is_file;
    cur_inode->size_of = size;
//printf("cur inode: %p\n", cur_inode);
}
void
inode_remove(inode* inode_ptr) {
}

// find an empty spot in inodes, insert the given inode, return the index of where the inode is stored or failure
// fixme, this func shouldn't have any input, because you have excess to all the addrs
//int
//inode_insert(inode* cur_inode, inode* inodes[], int inode_bitmap[]) {
//    int next_aval_index = inode_bitmap_find_next_empty(inode_bitmap);
//    // Used for debugging printf("next index %d\n", next_aval_index);
//    if (next_aval_index < 0) {
//        // operation failed due to lack of memory or disk space
//        return next_aval_index;
//    }
//    inodes[next_aval_index] = cur_inode;
//    // update bitmap
//    inode_bitmap[next_aval_index] = 1;
//    // success should return an int >= 0
//    return next_aval_index;
//}


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
    // return a value >= 0 if success, else return -ENOMEM for failure
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
