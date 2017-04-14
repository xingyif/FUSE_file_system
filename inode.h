#include <stdio.h>

typedef struct inode {
    int refs; // reference count
    int mode; // permission & type
    int size; // bytes for file
    int xtra; // more stuff can go here
} inode;

void pages_init(const char* path);
void pages_free();
void* pages_get_inode(int inum);
inode* pages_get(int node_id);
int pages_find_empty();
void print_inode(inode* node);
