#include "inode.h"
#include "storage.h"
#include <sys/stat.h>
#include <sys/mman.h>
#include <assert.h>
#include <stdlib.h>

int ROOT_DIR_IDX = 0;

//const int INODE_COUNT = 256;
//static void* inode_ptr =  0;
//static int   inode_fd   = -1;



inode*
inode_init() {
//    inode_fd = open(path, O_CREAT | O_RDWR, 0644);
//    assert(inode_fd != -1);

    // the regular file named by path or referenced by fd to be truncated to a size of precisely length bytes.
//    int rv = ftruncate(iblock_fd, NUFS_SIZE);
//    assert(rv == 0); // success

    inode* inode_ptr = malloc(sizeof(inode)); // mmap(0, sizeof(inode), PROT_READ | PROT_WRITE, MAP_SHARED, inode_fd, 0);
    assert(inode_ptr != MAP_FAILED);

    return inode_ptr;
    // put metadata into inode todo should this happen here
//    struct stat *buf;
//    stat(pathname, buf);
//    current_inode.mode = buf->st_mode;
//    current_inode.user_id = buf->st_uid;
//    current_inode.size_of = buf->st_size;
//    current_inode.size_of = is_file(pathname); // todo write a function in util to read last 3 char in given path, return 1 for file, 0 for dir
}

inode
inode_get(const char *pathname, inode inodes[]) {
//    inode current_inode = inodes[index];
    // if this is the first time
//    if (current_inode == NULL) { // todo should i use array of ptrs for inodes?
        // initialize inode
//        inode_init(pathname, current_inode);
//    }
    int next_aval_index = inode_bitmap_find_next_empty(inodes);
    return inodes[next_aval_index];
}

int
inode_bitmap_find_next_empty(int inode_bitmap[])
{
    int inode_index = -1;
    for (int ii = 2; ii < INODE_COUNT; ++ii) {
        if (inode_bitmap[ii] == 0) { // if iblock is empty
            inode_index = ii;
            break;
        }
    }
    return inode_index;
}