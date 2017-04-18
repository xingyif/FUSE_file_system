#include <string.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "inode.h"
#include "slist.h"
#include "util.h"
#include "iblock.h"

// todo rewrite this for init, free, and get(if null, init)

const int NUFS_SIZE  = 1024 * 1024; // 1MB
const int IBLOCK_SIZE = 1024 * 4; // = 4096 = 4k block
const int IBLOCK_COUNT = 256;

int iblock_bitmap[256];
void* iblocks[256]; 

static int   iblock_fd   = -1;
//static void* iblock_ptr =  0;

// initialize a single 4k block
iblock*
iblocks_init()
{
//    iblock_fd = open(path, O_CREAT | O_RDWR, 0644);
//    assert(iblock_fd != -1);

    // the regular file named by path or referenced by fd to be truncated to a size of precisely length bytes.
//    int rv = ftruncate(iblock_fd, NUFS_SIZE);
//    assert(rv == 0); // success

    iblock* iblock_ptr = malloc(IBLOCK_SIZE); // mmap(0, IBLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, iblock_fd, 0);
//    assert(iblock_ptr != MAP_FAILED);
    return iblock_ptr;
}

void
iblock_free(iblock* iblock_ptr)
{
    free(iblock_ptr); // munmap(iblock_ptr, IBLOCK_SIZE);
}

// find an empty spot in iblocks, insert the given iblock, return the index of where the iblock is stored or failure
int
iblock_insert(void* cur_iblock, void* iblocks[], int iblock_bitmap[])
{
    int next_aval_index = iblock_bitmap_find_next_empty(iblock_bitmap);
    if (next_aval_index < 0) {
        // operation failed due to lack of memory or disk space
        return next_aval_index;
    }
    iblocks[next_aval_index] = cur_iblock;
    // update bitmap
    iblock_bitmap[next_aval_index] = 1;
    return next_aval_index;
}

int
iblock_bitmap_find_next_empty(int iblock_bitmap[])
{
    int iblock_index = -ENOMEM; // opration failed due to lack of memory/disk space
    for (int ii = 2; ii < IBLOCK_COUNT; ++ii) {
        if (iblock_bitmap[ii] == 0) { // if iblock is empty
            iblock_index = ii;
            break;
        }
    }
    return iblock_index;
}
/*
void
print_node(inode* node)
{
    if (node) {
        printf("node{user_id: %d, mode: %04o, size: %d, is_file: %d}\n",
               node->user_id, node->mode, node->size_of, node->is_file);
    }
    else {
        printf("node{null}\n");
    }
}

*/


