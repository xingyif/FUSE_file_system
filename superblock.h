#ifndef SPRBLOCK_H
#define SPRBLOCK_H

// Created by Brett 4/13/2017
// Modified by Yifan & Brett
#include <stdio.h>

#include "storage.h"

typedef struct superblock {
	size_t ibitmap_location; // offset of inode bitmap
	size_t bbitmap_location; // offset of block bitmap
	size_t inodes; // offset of inodes
    size_t iblocks; // offset of iblocks
	int root_inode_idx; //root inode index this is the index of the root directory
} superblock;


void   superblock_init(void* disk_image);
superblock* superblock_addr();
#endif

