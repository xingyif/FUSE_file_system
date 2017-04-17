#ifndef SPRBLOCK_H
#define SPRBLOCK_H

// Created by Brett 4/13/2017
// Modified by Yifan & Brett
// import block.h inode.h pages.h
#include <stdio.h>

//#include "inode.h"
//#include "iblock.h"
#include "storage.h"

typedef struct superblock {
	int (*ibitmap_location)[256]; // location of inode bitmap
	int (*bbitmap_location)[256]; // location of block bitmap
	int num_of_inodes; // number of inodes
	inode (*inodes)[256]; // location to inodes
	int num_of_blocks; // number of blocks
    iblock (*blocks)[256]; // location to blocks
	int root_inode_idx; //root inode index this is the index of the root directory, rootdir is an inode,

} superblock;

extern superblock* sprblk;

void   superblock_init();
void   superblock_free();
void   print_superblock(superblock* superblock);
void superblock_add_inode(const char* path);
#endif

