// Created by Yifan

typedef struct iblock {
    char contents[4096]; // 4 * 1024
} iblock;
// directory[128], sizeof(directory) should == 4096, which is contents size
// when the inode is a dir, the block should contains a content, which has a directory

// not sure how many we need
void iblock_init(const char* path);
void iblock_free();
iblock iblock_get(iblock iblocks[], int iblock_bitmap[]);
int iblock_bitmap_find_next_empty(int iblock_bitmap[]);
void print_inode(inode* node);
