#include <string.h>
#include <errno.h>
#include "iblock.h"
#include "superblock.h"
#include "storage.h"

const int IBLOCK_SIZE = 1024 * 4; // = 4096 = 4k block
const int IBLOCK_COUNT = 256;


// initialize a single 4k block
void
iblock_init(iblock* cur_block)
{
//    iblock_fd = open(path, O_CREAT | O_RDWR, 0644);
//    assert(iblock_fd != -1);

    // the regular file named by path or referenced by fd to be truncated to a size of precisely length bytes.
//    int rv = ftruncate(iblock_fd, NUFS_SIZE);
//    assert(rv == 0); // success
}

//// find an empty spot in iblocks, insert the given iblock, return the index of where the iblock is stored or failure
//int
//iblock_insert(void* cur_iblock, void* iblocks[], int iblock_bitmap[])
//{
//    int next_aval_index = iblock_bitmap_find_next_empty(iblock_bitmap);
//    if (next_aval_index < 0) {
//        // operation failed due to lack of memory or disk space
//        return next_aval_index;
//    }
//    iblocks[next_aval_index] = cur_iblock;
//    // update bitmap
//    iblock_bitmap[next_aval_index] = 1;
//    return next_aval_index;
//}

int
iblock_bitmap_find_next_empty(int* iblock_bitmap_ptr)
{
    int iblock_index = -ENOMEM; // opration failed due to lack of memory/disk space
    for (int ii = 1; ii < IBLOCK_COUNT; ++ii) {
        if (iblock_bitmap_ptr[ii] == 0) { // if iblock is empty
            iblock_index = ii;
            break;
        }
    }
    return iblock_index;
}

void**
iblocks_addr() {
    return (void**) get_disk() + superblock_addr()->iblocks;
}

int*
iblock_bitmap_addr() {
    return (int*) get_disk() + superblock_addr()->bbitmap_location;
}

void*
single_iblock_addr(int idx) {
    return (void*) (iblocks_addr() + 4096 * idx);
}
