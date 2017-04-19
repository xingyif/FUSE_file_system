
#include <string.h>

#include "superblock.h"
//import blocks
//import pages
//import inode

//superblock* sprblk;

void
superblock_init()
{
    // offset for the superblock
    size_t offset = 0;

    // todo not sure if i need this
    superblock* sprblk = disk;

    // offset for inode_bitmap
    offset += sizeof(superblock);
    sprblk->ibitmap_location = offset;

    // offset for iblock_bitmap
    offset += 256 * sizeof(char); // use char => 1byte, int => 8bytes, saves space in "disk"
    sprblk->bbitmap_location = offset;

    // offset for inodes
    offset += 256 * sizeof(char); // use char => 1byte, int => 8bytes, saves space in "disk"
    sprblk->inodes = offset;

    // offset for iblocks
    offset += 256 * sizeof(inode);
    sprblk->iblocks = offset;

	sprblk->root_inode_idx = 0;
}

void
superblock_free()
{
   // int rv = munmap(pages_base, NUFS_SIZE);
    // todo close?
   // assert(rv == 0);
}

void
print_superblock(superblock* superblock)
{/*
    if (node) {
        printf("node{refs: %d, mode: %04o, size: %d, xtra: %d}\n",
               node->refs, node->mode, node->size, node->xtra);
    }
    else {
        printf("node{null}\n");
    }*/
}


/* This si the function that looks for an empty space in a bitmap


int
find_empty_ibitmap() {
int empt = 0;

for (

}
*/

void
superblock_add_inode(const char* path) {
//look in bitmap for free location
//add inode to direcotry
//mark in bitmap that free location is used
}

superblock*
superblock_addr() {
    return (superblock*) disk;
}

