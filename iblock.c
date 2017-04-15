#include <string.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

#include "inode.h"
#include "slist.h"
#include "util.h"

// todo rewrite this for init, free, and get(if null, init)

const int NUFS_SIZE  = 1024 * 1024; // 1MB
const int IBLOCK_COUNT = 256;

static int   iblock_fd   = -1;
static void* iblock_base =  0;

void
iblock_init(const char* path)
{
    iblock_fd = open(path, O_CREAT | O_RDWR, 0644);
    assert(iblock_fd != -1);

    int rv = ftruncate(iblock_fd, NUFS_SIZE);
    assert(rv == 0);

    iblock_base = mmap(0, NUFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, iblock_fd, 0);
    assert(iblock_base != MAP_FAILED);
}

void
iblock_free()
{
    int rv = munmap(iblock_base, NUFS_SIZE);
    assert(rv == 0);
}

void*
iblock_get_inode(int inum)
{
    return iblock_base + 4096 * inum;
}

inode*
iblock_get(int node_id)
{
    inode* idx = (inode*) iblock_get_inode(0);
    int inum = iblock_find_empty();
    return &(idx[inum]);
}

int
iblock_find_empty()
{
    int pnum = -1;
    for (int ii = 2; ii < IBLOCK_COUNT; ++ii) {
        if (0) { // if iblock is empty
            pnum = ii;
            break;
        }
    }
    return pnum;
}

void
print_node(inode* node)
{
    if (node) {
        printf("node{refs: %d, mode: %04o, size: %d, xtra: %d}\n",
               node->refs, node->mode, node->size, node->xtra);
    }
    else {
        printf("node{null}\n");
    }
}




