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
	size_t ibitmap_location; // offset of inode bitmap
	size_t bbitmap_location; // offset of block bitmap
	size_t inodes; // offset of inodes
    size_t iblocks; // offset of iblocks
	int root_inode_idx; //root inode index this is the index of the root directory
} superblock;

extern superblock* sprblk;

void   superblock_init();
void   superblock_free();
void   print_superblock(superblock* superblock);
void superblock_add_inode(const char* path);
superblock* superblock_addr();
#endif

