// Created by Yifan

typedef struct block {
    char contents[4096]; // 4 * 1024
} block;
// directory[128], sizeof(directory) should == 4096, which is contents size
// when the inode is a dir, the block should contains a content, which has a directory
