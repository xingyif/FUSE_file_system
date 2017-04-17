// ATTENTION: this should be where everything {superblock, bitmaps, inodes, iblocks} exist
// todo, init individual inodes/blocks when creating data, in nufs.c, only call those init functions, write init here
// this file should be the middleware => operation file

#include <stdio.h>
#include <string.h>

#include "util.h"

#include "storage.h"
typedef struct file_data {
    const char* path;
    int         mode;
    const char* data;
} file_data;

// todo when to put metadata to inode & when to put data to iblock???
//static file_data file_table[] = {
//    {"/", 040755, 0},
//    {"/hello.txt", 0100644, "hello\n"},
//    {0, 0, 0},
//};


// todo nufs.c should still call storage_init in main
void
storage_init(char* path)
{ 
    // initialize superblock
	superblock_init();
    // bitmaps initilized, fixed sized in storage.h
    // inodes initilized, fixed sized in storage.h
    // iblocks initilized, fixed sized in storage.h
    // create the root dir and put it to inodes and iblocks
    slist* path_list = s_split(path, '/'); // todo get home dir from array
//    char* path_array = slist_close(path_list);
    directory* root_dir = directory_init(path_list->data); // return the 0 index from the arr
    inode* root_inode = inode_init();
    iblock* root_iblock = iblock_init();

    inodes[sprblk->root_inode_idx] = root_inode;
    iblocks[sprblk->root_inode_idx] = root_dir;

    // mark the root inode & block to be used
    inode_bitmap[sprblk->root_inode_idx] = 1;
    iblock_bitmap[sprblk->root_inode_idx] = 1;
    printf("TODO: Store file system data in: %s\n", path);
}

void*
get_entry_block(char* path) {
    // 1. truncate path
    // 2. get inodes
    // 3. get iblocks
    slist* path_list = s_split(path, '/');//  get given dir/file from array
//    char* path_array = slist_close(path_list); don't need to use  slist_close returns a pointer to the array
    //todo check if user path starts at home else look at cur_dir path from home
    directory* cur_dir = (directory*) iblocks[sprblk->root_inode_idx]; // todo don't know if this works
    //todo assuming that user is giving path that either starts with home dir or entry in home dir
    if(streq(path_list->data, cur_dir->dir_name)) { //is the user's path starting from this directory or an entry in it
        path_list = path_list->next;
    }
    else {//todo ask prof tuck if this okie
        perror("user must give path that starts from home\n");
    }
    while(path_list != NULL) {
        int entry_inode_index = directory_lookup(cur_dir, path_list->data);
        if (entry_inode_index == -1) {
//            perror("can't find block\n");
  		return NULL;
        }
        else {
            dir_ent* cur_entry = cur_dir->entries[entry_inode_index];
            if (streq(cur_entry->filename, path_list->data)){
                return iblocks[cur_entry->entry_inode_index];
            }
            else {
                cur_dir = (directory*) iblocks[entry_inode_index];
                path_list = path_list->next;
            }
        }
    }

    return NULL;
}

int
get_stat(char* path, struct stat* st)
{
    inode* dat = get_entry_block(path);
    if (!dat) {
        return -1;
    }

    memset(st, 0, sizeof(struct stat));
    st->st_uid  = dat->user_id; // getuid();
    st->st_mode = dat->mode;

    st->st_size = dat->size_of;
    return 0;
}

const char*
get_data(char* path)
{
    // assuming that the given path is to a file not a directory
    iblock* dat = get_entry_block(path);
    return dat->contents;
}

