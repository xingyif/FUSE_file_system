// ATTENTION: this should be where everything {superblock, bitmaps, inodes, iblocks} exist
// todo, init individual inodes/blocks when creating data, in nufs.c, only call those init functions, write init here
// this file should be the middleware => operation file
// created by Nat Tuck
// mofied by Yifan & Brett
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
//#include "superblock.h"
//#include "directory.h"

#include "util.h"
//#include "iblock.h"
#include "storage.h"
#include "pages.h"

typedef struct file_data {
    const char* path;
    int         mode;
    const char* data;
} file_data;
const int DISK_SIZE  = 1024 * 1024; // 1MB
void* disk;
//superblock* sprblk;
// todo when to put metadata to inode & when to put data to iblock???
//static file_data file_table[] = {
//    {"/", 040755, 0},
//    {"/hello.txt", 0100644, "hello\n"},
//    {0, 0, 0},
//};


// todo nufs.c should still call storage_init in main
void
storage_init(char* disk_image)
{
    
	printf("home path: %s\n", disk_image);
   // pages_init(disk_image);




    int fd;
    if ((fd = open(disk_image, O_CREAT | O_RDWR, 0644)) == -1) {
        perror("Opening disk image failed!");
        exit(1);
    }
        perror("? ");
	printf("file descriptor maid\n");

	int rv = ftruncate(fd, DISK_SIZE);
assert(rv == 0);

    // returns a non-negative integer, termed a file descriptor.  It returns -1 on failure, and sets
    // errno to indicate the error return - value if failed
    // initialize superblock if it has never been initialized before
    disk = mmap(NULL, DISK_SIZE , PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (disk == MAP_FAILED) {
      perror("Couldn't map image");
      exit(1);
    }
	printf("mmaped\n");
//sprblk = (superblock *) disk;

// Had to comment out to try and avoid this error

//    if (superblock_addr()->ibitmap_location == NULL) { // todo ? check a field, because sprblk_addr = disk
	printf("superblock making time\n");
	    superblock_init(disk);


    // bitmaps initilized, fixed sized in storage.h
    // inodes initilized, fixed sized in storage.h
    // iblocks initilized, fixed sized in storage.h
    // create the root dir and put it to inodes and iblocks
    printf("Store file system data in: %s\n", disk_image);
    printf("Disk address is at: %p\n", disk);

    // setting up root_dir inode
    superblock* sprblk_addr = superblock_addr();
    int root_dir_idx = sprblk_addr->root_inode_idx;
    // get inode* from inodes
    inode* root_inode = inodes_addr()[root_dir_idx];
    inode_init(root_inode, S_IRWXU | S_IRWXG | S_IRWXO, 0, 128);
    // update inode* in inodes
    inodes_addr()[root_dir_idx] = root_inode;

    // setting up root_dir block
    void* root_directory = iblocks_addr()[root_dir_idx];
    char* root_dir_name = "/mnt";
    // get dir* from iblocks and initialize the root_dir
    directory_init(root_directory, root_dir_name);
    // update dir* in iblocks
    iblocks_addr()[root_dir_idx] = root_directory;


//    slist* path_list = s_split(path, '/'); // todo get home dir from array
    // \\\\\\ char* path_array = slist_close(path_list);
//	while(path_list != NULL) {
//	printf("home path: %s\n", path_list->data);
//	path_list = path_list->next;
//}
    // \\\\\\ directory* root_dir = directory_init(path_list->data); // return the 0 index from the arr
	//todo check the size to put in here
//    inode* root_inode = inode_init(S_IRWXU | S_IRWXG | S_IRWXO, 0, 128);
    // \\\\\\ iblock* root_iblock = iblock_init();

    // \\\\\\ int rv_inode = inode_insert(root_inode, inodes, inode_bitmap);
//    inodes[sprblk->root_inode_idx] = root_inode;
    // \\\\\\ make sure the inode is inserted into index 0 in inodes
    // \\\\\\  assert(rv_inode == 0);

    // \\\\\\\ int rv_iblock = iblock_insert(root_dir, iblocks, iblock_bitmap);
//    iblocks[sprblk->root_inode_idx] = root_dir;
    // \\\\\\\ assure that root_dir is inserted at index 0 in iblocks
    // \\\\\\\ assert(rv_iblock == 0);

    // mark the root inode & block to be used
//    inode_bitmap[sprblk->root_inode_idx] = 1;
//    iblock_bitmap[sprblk->root_inode_idx] = 1;
}

int
get_entry_index(char *path) {
    // 1. truncate path
    // 2. get inodes
    // 3. get iblocks
    slist* path_list = s_split(path, '/');//  get given dir/file from array
    //  char* path_array = slist_close(path_list); don't need to use  slist_close returns a pointer to the array
    //todo check if user path starts at home else look at cur_dir path from home
    // fixme addr() returns ** because can't case void to directory
    directory* cur_dir = (directory*) (iblocks_addr()[superblock_addr()->root_inode_idx]); // todo don't know if this works
    //todo assuming that user is giving path that either starts with home dir or entry in home dir
    if(streq(path_list->data, cur_dir->dir_name)) { //is the user's path starting from this directory or an entry in it
        path_list = path_list->next;
    }
    /*
    else {//todo ask prof tuck if this oki
	printf("current directory: %s\n", cur_dir->dir_name);
	printf("path list: %s\n", path_list->data);
        perror("user must give path that starts from home\n");
    }*/
    while(path_list != NULL) {
        int entry_inode_index = directory_lookup(cur_dir, path_list->data);
        if (entry_inode_index == -1) {
        // perror("can't find block\n");
  		return -ENOENT;
        }
        else {
            dir_ent* cur_entry = cur_dir->entries[entry_inode_index];
            if (streq(cur_entry->filename, path_list->data)){
                return cur_entry->entry_inode_index;
            }
            else {
                cur_dir = (directory*) iblocks_addr()[entry_inode_index];
                path_list = path_list->next;
            }
        }
    }

    return -ENOENT;
}

int
get_stat(char* path, struct stat* st)
{

    int index = get_entry_index(path);
    if (index < 0) {
        // didn't find the given path
        //todo perror&exit or return errorCode????????????????
        return -ENOENT; // TODO included <errno.h>
    }
    // write sizeof(stat) bytes of 0 to st
    memset(st, 0, sizeof(struct stat));

    inode cur_inode = inodes_addr()[index];
    st->st_uid  = cur_inode.user_id;
    st->st_mode = cur_inode.mode;

    st->st_size = cur_inode.size_of;
    return 0;
}

const char*
get_data(char* path) // todo do we always assume the path is a file????????????
{
    // assuming that the given path is to a file not a directory
    int index = get_entry_index(path);
    if (index < 0) {
        return -ENOENT;
    }
    iblock* cur_iblock = iblocks_addr()[index];
    return cur_iblock->contents;
}

//const char*
//get_data(char* path, int is_file)
//{
//    // assuming that the given path is to a file not a directory
//    int index = get_entry_index(path);
//    if (index < 0) {
//        return -ENOENT;
//    }
//    iblock* cur_iblock = iblocks_addr()[index];
//    directory* cur_dir = iblocks_addr()[index];
//
//    // if path = file
//    if (is_file == 1) {
//          return cur_iblock->contents;
//    } else {
//          return cur_dir->entries;
//    }
//}

void*
get_disk() {
	return disk;
}
