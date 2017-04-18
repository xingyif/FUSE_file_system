#include "inode.h"
//#include "storage.h"
#include <sys/stat.h>
#include <sys/mman.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int ROOT_DIR_IDX = 0;

//const int INODE_COUNT = 256;
//static void* inode_ptr =  0;
//static int   inode_fd   = -1;

inode* inodes[256];
int inode_bitmap[256];

inode*
inode_init(mode_t mode, int is_file, size_t size) {
//    inode_fd = open(path, O_CREAT | O_RDWR, 0644);
//    assert(inode_fd != -1);

    // the regular file named by path or referenced by fd to be truncated to a size of precisely length bytes.
//    int rv = ftruncate(iblock_fd, NUFS_SIZE);
//    assert(rv == 0); // success

    inode* inode_ptr = malloc(sizeof(inode)); // mmap(0, sizeof(inode), PROT_READ | PROT_WRITE, MAP_SHARED, inode_fd, 0);
//    assert(inode_ptr != MAP_FAILED);

    inode_ptr->user_id = getuid();
    inode_ptr->mode = mode;
    inode_ptr->is_file = is_file;
    inode_ptr->size_of = size;
    return inode_ptr;
}
void
inode_free(inode* inode_ptr) {
   free(inode_ptr);
}
// find an empty spot in inodes, insert the given inode, return the index of where the inode is stored or failure
int
inode_insert(inode* cur_inode, inode* inodes[], int inode_bitmap[]) {
    int next_aval_index = inode_bitmap_find_next_empty(inode_bitmap);
    printf("next index %d\n", next_aval_index);
    if (next_aval_index < 0) {
        // operation failed due to lack of memory or disk space
        return next_aval_index;
    }
    inodes[next_aval_index] = cur_inode;
    // update bitmap
    inode_bitmap[next_aval_index] = 1;
    // success should return an int >= 0
    return next_aval_index;
}


int
inode_bitmap_find_next_empty(int inode_bitmap[])
{
    int inode_index = -ENOMEM; // operation failed due to lack of memory or disk space
    for (int ii = 0; ii < 256; ++ii) {
        if (inode_bitmap[ii] == 0) { // if iblock is empty
            inode_index = ii;
            break;
        }
    }
    // return a value >= 0 if success, else return -ENOMEM for failure
    return inode_index;
}
