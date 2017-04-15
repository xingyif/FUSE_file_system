#include <stdio.h>

typedef struct inode {
    char mode[3]; // permission & type
    int user_id; // can be uint16_t, who created this file
    int size_of; // size of file/dir in bytes
    int is_file; // flag, if obj is file = 1, dir = 0
} inode;
void inode_init(const char *pathname, inode current_inode);
inode get_inode(const char *pathname, inode inodes[256], int index);