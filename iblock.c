#include <string.h>
#include <errno.h>
#include "iblock.h"
#include "superblock.h"
#include "storage.h"

const int IBLOCK_SIZE = 1024 * 4; // = 4096 = 4k block
const int IBLOCK_COUNT = 256;

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
