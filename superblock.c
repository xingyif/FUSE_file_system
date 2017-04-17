
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

#include "superblock.h"
#include "storage.h"
//import blocks
//import pages
//import inode

const int SUPERBLOCK_SIZE  = 1024 * 1024; // 1MB


void
superblock_init()
{
	sprblk = malloc(SUPERBLOCK_SIZE); // mmap(0, SUPERBLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, -1, 0);
    assert(sprblk != 0);
	sprblk->ibitmap_location = &inode_bitmap;
	sprblk->bbitmap_location = &iblock_bitmap;
	sprblk->num_of_inodes = INODE_COUNT;
    sprblk->num_of_blocks = INODE_COUNT;
    sprblk->inodes = &inodes;
    sprblk->blocks = &iblocks;
	sprblk->root_inode_idx = 0;
}

void
superblock_free()
{
   // int rv = munmap(pages_base, NUFS_SIZE);
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



