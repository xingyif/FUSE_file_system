#ifndef NUFS_STORAGE_H
#define NUFS_STORAGE_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

// todo we don't need these below to be global, because we can use ..._addr()
//extern superblock* sprblk;
//extern int inode_bitmap[256];
//extern int iblock_bitmap[256];
//extern inode* inodes[256];
//extern void* iblocks[256]; // void* because it can be an iblock/directory
//extern int INODE_COUNT = 256; // extern variable can't have an initializer
//extern void* disk;

void storage_init(char* path);
int get_entry_index(char *path);
int add_dir_entry(char *path, int new_inode_idx);
int         get_stat(char* path, struct stat* st);
void* get_data(char* path);
void* get_disk();
#endif
