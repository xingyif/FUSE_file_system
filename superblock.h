#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H

 Made by Brett 4/13/2017

#include <stdio.h>

typedef struct superblock {
uint64_t magic;
uint64_t inode_map_size; /* in 1024-byte blocks */
uint64_t inode_region_sz; /* in 1024-byte blocks */
uint64_t block_map_sz; /* in 1024-byte blocks */
uint64_t num_blocks; /* total disk size */ 
uint64_t root_inode; 
padding[];/* to make size = 1024 */
} 

void*   superblock_init();
void   superblock_free();
superblock* super_get_block(int block_id);
void   print_superblock(superblock* superblock);

#endif

