
#include <string.h>

#include "superblock.h"
#include "inode.h"
#include "iblock.h"

static superblock *sprblk;

void
superblock_init(void *disk_img) {
    // has_set_sprblk = 1;
    // offset for the superblock
    size_t offset = 0;
    // todo not sure if i need this
    sprblk = disk_img;

    // offset for inode_bitmap
    offset += sizeof(superblock);
    sprblk->ibitmap_location = offset;

    // offset for iblock_bitmap
    offset += 256 * sizeof(char); // use char => 1byte, int => 8bytes, saves space in "disk"
    sprblk->bbitmap_location = offset;

    //printf("making the superblock 4\n");
    // offset for inodes
    offset += 256 * sizeof(char); // use char => 1byte, int => 8bytes, saves space in "disk"
    sprblk->inodes = offset;

    //printf("making the superblock 5\n");
    // offset for iblocks
    offset += 256 * sizeof(inode);
    sprblk->iblocks = offset;

    //printf("making the superblock 6\n");
    sprblk->root_inode_idx = 0;
}

superblock *
superblock_addr() {
    return sprblk;
}
