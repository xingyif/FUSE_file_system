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

#include "directory.h"
#include "superblock.h"
#include "inode.h"
#include "util.h"
#include "iblock.h"
#include "storage.h"
#include "pages.h"
#include "slist.h"

typedef struct file_data {
    const char *path;
    int mode;
    const char *data;
} file_data;
const int DISK_SIZE = 1024 * 1024; // 1MB
void *disk;
//superblock* sprblk;
// todo when to put metadata to inode & when to put data to iblock???
//static file_data file_table[] = {
//    {"/", 040755, 0},
//    {"/hello.txt", 0100644, "hello\n"},
//    {0, 0, 0},
//};


// todo nufs.c should still call storage_init in main
void
storage_init(char *disk_image) {

    printf("in storage_init, disk_image: %s\n", disk_image);
    // pages_init(disk_image);




    int fd;
    if ((fd = open(disk_image, O_CREAT | O_RDWR, 0644)) == -1) {
        perror("Opening disk image failed!");
        exit(1);
    }
    perror("? ");
  //  printf("file descriptor maid\n");

    int rv = ftruncate(fd, DISK_SIZE);
    assert(rv == 0);

    // returns a non-negative integer, termed a file descriptor.  It returns -1 on failure, and sets
    // errno to indicate the error return - value if failed
    // initialize superblock if it has never been initialized before
    disk = mmap(NULL, DISK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (disk == MAP_FAILED) {
        perror("Couldn't map image");
        exit(1);
    }
    printf("in storage_init, mmaped the disk\n");
//sprblk = (superblock *) disk;

// Had to comment out to try and avoid this error

//    if (superblock_addr()->ibitmap_location == NULL) { // todo ? check a field, because sprblk_addr = disk
   // printf("superblock making time\n");
    superblock_init(disk);


    // bitmaps initilized, fixed sized in storage.h
    // inodes initilized, fixed sized in storage.h
    // iblocks initilized, fixed sized in storage.h
    // create the root dir and put it to inodes and iblocks
   // printf("Store file system data in: %s\n", disk_image);
   // printf("Disk address is at: %p\n", disk);
    printf("in storage_init, disk_image: %s\n", disk_image);
    printf("in storage_init, disk_ptr: %p\n", disk);

    // setting up inode_bitmap and iblock_bitmap
    for (int i = 0; i < 256; i++) {
        inode_bitmap_addr()[i] = 0;
        iblock_bitmap_addr()[i] = 0;
    }

    // setting up root_dir inode
    superblock *sprblk_addr = superblock_addr();
    int root_dir_idx = sprblk_addr->root_inode_idx;
    // get inode* from inodes
    inode *root_inode = single_inode_addr(root_dir_idx);

   // printf("root inode is: %p\n", root_inode);
   inodes_addr()[root_dir_idx] = root_inode;
    inode_init(root_inode, 040755, 0, 4096); // S_IRWXU | S_IRWXG | S_IRWXO

   // printf("root inode pointer 2 is: %p\n", single_inode_addr(0));

    printf("in storage_init, root inode ptr: %p\n", root_inode);
//    inodes_addr()[root_dir_idx] = root_inode;
   // inode_init(root_inode, 040755, 0, 4096); // S_IRWXU | S_IRWXG | S_IRWXO
   // inodes_addr()[root_dir_idx] = root_inode;

    // update inode* in inodes
    inode_bitmap_addr()[root_dir_idx] = 1;

   // printf("root inode pointer 1 is: %p\n", inodes_addr()[root_dir_idx]);

    //creating iblock root_dir here
    directory *root_iblock = single_iblock_addr(root_dir_idx);
//    iblocks_addr()[root_dir_idx] = root_iblock;

    char *root_dir_name = '/'; //todo is it mnt or '/'
    // get dir* from iblocks and initialize the root_dir
    directory_init(root_iblock, root_dir_name);
    iblocks_addr()[root_dir_idx] = root_iblock;
    iblock_bitmap_addr()[root_dir_idx] = 1;
    //printf("root inode pointer is: %p\n", inodes_addr()[root_dir_idx]);

    // setting up root_dir block
    // update dir* in iblocks



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
    if (*path == '/') {
/*todo merge conflict
      //  printf("home dir\n");
        return 0;
    }
    slist *path_list = s_split(path, '/');//  get given dir/file from array
   // printf("home path: %s\n", path_list->data);
*/
        printf("in get_entry_index, given path is home dir\n");
        return 0;
    }
    slist *path_list = s_split(path, '/');//  get given dir/file from array
    printf("in get_entry_index, home path: %s\n", path_list->data);

    //  char* path_array = slist_close(path_list); don't need to use  slist_close returns a pointer to the array
    //todo check if user path starts at home else look at cur_dir path from home
    // fixme addr() returns ** because can't case void to directory
    int current_inode_idx = superblock_addr()->root_inode_idx;
    directory *root_dir = (directory *) (iblocks_addr()[current_inode_idx]);
    //todo assuming that user is giving path that either starts with home dir or entry in home dir
    // get to the name we are looking for
    if (streq(path_list->data, root_dir->dir_name)) {
        path_list = path_list->next;
    }
    /*
    else {
	printf("current directory: %s\n", cur_dir->dir_name);
	printf("path list: %s\n", path_list->data);
        perror("user must give path that starts from home\n");
    }*/
    directory* current = root_dir;
    while (path_list != NULL) {
        // get the index of the entry
        int entry_idx = directory_entry_lookup(current, path_list->data);
        // didn't find the entry
        if (entry_idx < 0) {
            return -ENOENT; // no such file or dir
        }
        // get current entry from current dir
        dir_ent* cur_ent = current->entries[entry_idx];
        int entry_inode_index = cur_ent->entry_inode_index;

        // find the file
        if (path_list->next == NULL) {
            return cur_ent->entry_inode_index;
        } else {
            // current is not a file
            current = iblocks_addr()[entry_inode_index];
            path_list = path_list->next;
        }
    }
    return -ENOENT;
}

int
add_dir_entry(char *path, int new_inode_idx) {
    slist *path_list = s_split(path, '/');
    directory *root_dir = (directory *) (iblocks_addr()[superblock_addr()->root_inode_idx]);

    // if in root dir, move path_list to the next
    if (streq(path_list->data, root_dir->dir_name)) {
        path_list = path_list->next;
    }

    directory *current = root_dir;
    while (path_list != NULL) {
        // get the index of the entry
        int entry_idx = directory_entry_lookup(current, path_list->data);
        // didn't find the entry, and the path_list is the last in the list
        // make a new entry and put it in cur_dir
        if ((entry_idx < 0) && (path_list->next == NULL)) {
            // put the entry array in
            int new_entry_idx = directory_insert_entry(current, path_list->data, new_inode_idx);
            return new_entry_idx; // success or didn't successfully insert
        } else {
            // if didn't find it, and the next is not null, throw path doesn't exist exception
            if (entry_idx < 0) {
                return -ENOENT;
            }
            // haven't finished yet, keep traversing
            dir_ent *cur_ent = current->entries[entry_idx];
            int entry_inode_index = cur_ent->entry_inode_index;
            current = iblocks_addr()[entry_inode_index];
            path_list = path_list->next;
        }
    }
    return -ENOENT;
}


int
remove_dir_entry(char *path) {
    slist *path_list = s_split(path, '/');
    directory *root_dir = (directory *) (iblocks_addr()[superblock_addr()->root_inode_idx]);

    // if in root dir, move path_list to the next
    if (streq(path_list->data, root_dir->dir_name)) {
        path_list = path_list->next;
    }

    directory *current = root_dir;
    while (path_list != NULL) {
        // get the index of the entry
        int entry_idx = directory_entry_lookup(current, path_list->data);
        // didn't find current entry
        // return path not found error
        if (entry_idx < 0) {
            return -ENOENT;
        }
        // found it, and the path_list is the last in the list, delete it
        if (path_list->next == NULL) {
            int rv = directory_del_entry(current, entry_idx);
            return rv; // either failure or success
        } else {
            // haven't finished yet, keep traversing
            dir_ent *cur_ent = current->entries[entry_idx];
            int entry_inode_index = cur_ent->entry_inode_index;
            current = iblocks_addr()[entry_inode_index];  // todo should i switch this to single_iblock...
            path_list = path_list->next;
        }
    }
    return -ENOENT;
}

int
get_stat(char *path, struct stat *st) {
   // printf("in get_stat\n");
    int index = get_entry_index(path);
    if (index < 0) {
        // didn't find the given path
        printf("in get_stat, about to return ENOENT, because given path doesn't exist\n");
        return -ENOENT;
    }
    // write sizeof(stat) bytes of 0 to st
    memset(st, 0, sizeof(struct stat));

    inode *cur_inode = (inode *) single_inode_addr(index);
    st->st_uid = cur_inode->user_id;
    st->st_mode = cur_inode->mode;

    st->st_size = cur_inode->size_of;
    return 0;
}

void*
get_data(char *path) // todo do we always assume the path is a file???????????? no
{
    // assuming that the given path is to a file not a directory
    int index = get_entry_index(path);
    if (index < 0) {
        return -ENOENT;
    }
    inode* cur_inode = inodes_addr()[index];
    // if we are looking at a file
    if (cur_inode->is_file) {
        iblock *cur_iblock = iblocks_addr()[index];
        return cur_iblock->contents;
    }
    // if we are looking at a directory
    directory* cur_dir = iblocks_addr()[index];
    return cur_dir;
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

void *
get_disk() {
    return disk;
}

//void
//flush_to_disk(void**)
