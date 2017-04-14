
#include <string.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

#include "superblock.h"
//import blocks
//import pages
//import inode
// BRETT IS STILL WORKING

const int SUPERBLOCK_SIZE  = 1024 * 1024; // 1MB
const int INODE_COUNT = 256;


void*
superblock_init()
{
	superblock *sprblk = mmap(0, SUPERBLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, -1, 0);
    assert(sprblk != 0);
	
	sprblk->inode_map_size = INODE_COUNT;
	sprblk->ibitmap_location = NULL; // needs to be overriden
	
	sprblk->block_map_size = INODE_COUNT;
	sprblk->bbitmap_location = NULL; // needs to be overridden

	sprblk->num_of_inodes = INODE_COUNT;
	sprblk->inodes = NULL; //needs to be overriden

	sprblk->num_of_blocks = INODE_COUNT;
//not work	sprblk->blocks = NULL; //needs to be overriden
	
	sprblk->root_inode_idx = 0;

	return sprblk;	
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



