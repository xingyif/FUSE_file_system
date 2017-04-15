#include "inode.h"
#include <sys/stat.h>
int ROOT_DIR_IDX = 0;

void
inode_init(const char *pathname, inode current_inode) {
//    mode[3];
    struct stat *buf;
    stat(pathname, buf);
    current_inode.mode = buf->st_mode;
    current_inode.user_id = buf->st_uid;
    current_inode.size_of = buf->st_size;
    current_inode.size_of = is_file(pathname); // todo write a function in util to read last 3 char in given path, return 1 for file, 0 for dir
}

inode
get_inode(const char *pathname, inode inodes[256], int index) {
    inode current_inode = inodes[index];
    // if this is the first time
    if (current_inode == NULL) {
        // initialize inode
        inode_init(current_inode);
    }
    return current_inode;
}