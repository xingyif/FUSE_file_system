// Created by Yifan

typedef struct iblock {
    char contents[4096]; // 4 * 1024
} iblock;
// directory[128], sizeof(directory) should == 4096, which is contents size
// when the inode is a dir, the block should contains a content, which has a directory

// not sure how many we need
void iblock_init(const char* path);
void iblock_free();
void* iblock_get_inode(int inum);
inode* iblock_get(int node_id);
int iblock_find_empty();
void print_inode(inode* node);
