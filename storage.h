#ifndef NUFS_STORAGE_H
#define NUFS_STORAGE_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

void storage_init(char* path);
int get_entry_index(char *path);
int add_dir_entry(char *path, int new_inode_idx);
int remove_dir_entry(char *path);
int get_stat(char* path, struct stat* st);
void* get_data(char* path);
void* get_disk();
void storage_free();
#endif
