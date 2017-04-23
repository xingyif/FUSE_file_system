// Created by Brett
// Modified by Yifan & Brett
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <memory.h>
#include "directory.h"
#include "slist.h"
#include "util.h"

const int FILE_NAME_LENGTH = 27;
const int DIR_ENT_SIZE = 32;

void
directory_init(directory *cur_dir, char *name) {
    if (sizeof(name) > FILE_NAME_LENGTH) {
        printf("Name of file/dir is too long!");
        return;
    }
    cur_dir->dir_name = name;
    cur_dir->number_of_entries = 0;
    // entries are fixed size array
}

// look for the entry index of a name inside a dir
int
directory_entry_lookup(directory *dir, char *name) {
    int num_of_entries = dir->number_of_entries;
    for (int i = 0; i < DIR_ENT_SIZE; i++) {
        dir_ent current_entry = dir->entries[i];
        // if current_try == NULL
        if (current_entry.filename == NULL) {
            continue;
        }
        if (streq(current_entry.filename, name)) {
            return i;
        }
    }
    return -ENOENT; // didn't find one
}

//given a directory, name of entry,
int
directory_insert_entry(directory *dir, char *name, int inode_index) {
    int num_of_entries = dir->number_of_entries;
    if (num_of_entries >= DIR_ENT_SIZE) {
        return -ENOSPC;
    }
    // check if the size of the char array is > 27
    if (sizeof(*name) > FILE_NAME_LENGTH) {
        return -ENOSPC;
    }
    // initialize the entry
    dir_ent new_entry;
    new_entry.filename = name;
    new_entry.entry_inode_index = inode_index;
    // find an empty spot in entries[], and insert it
    for (int i = 0; i < DIR_ENT_SIZE; i++) {
        dir_ent current_entry = dir->entries[i];
        // insert new entry
        // if current_entry == NULL
        if (current_entry.filename == NULL) {
            dir->entries[i] = new_entry;
            dir->number_of_entries++;
            return i;
        }
    }
    return -ENOSPC; // error: didn't successfully insert
}

// and inode_index of entry put an entry at an index return true if success
//delete an entry in a directory
int
directory_del_entry(directory *dir, int entry_idx) {
    int num_of_entries = dir->number_of_entries;
    if (num_of_entries <= 0) {
        return -ENOENT;
    }
    dir_ent cur_entry = dir->entries[entry_idx];
    memset(cur_entry.filename, 0, FILE_NAME_LENGTH);
    dir->number_of_entries--;
    return 0;
}
