#ifndef SPRBLOCK_H
#define SPRBLOCK_H

// Created by Brett 4/13/2017
// Modified by Yifan & Brett
// import block.h inode.h pages.h
#include <stdio.h>

#include "inode.h"
#include "iblock.h"

typedef struct superblock {
	// int inode_map_size; // size of inode bitmap
	int ibitmap_location[256]; // location of inode bitmap
	// int block_map_size; // block map size
	int bbitmap_location[256]; // location of block bitmap
	int num_of_inodes; // number of inodes
	inode inodes[256]; // location to inodes
	int num_of_blocks; // number of blocks
        iblock* blocks; // location to blocks
	int root_inode_idx; //root inode index this is the index of the root directory, rootdir is an inode,

} superblock;
void*   superblock_init();
void   superblock_free();
void   print_superblock(superblock* superblock);
void superblock_add_inode(const char* path);
#endif

