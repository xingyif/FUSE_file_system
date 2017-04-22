// Created by Brett
// Modified by Yifan & Brett
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <memory.h> // imported for memset in dir_del_entry
#include "directory.h"
#include "slist.h"
#include "util.h"

const int FILE_NAME_LENGTH = 27;
const int DIR_ENT_SIZE = 32;

void
directory_init(directory *cur_dir, char *name) {
    // todo check if the size of the char array is > 27
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
    printf("in directory_entry_lookup, given dir(%s), entry_name(%s)\n", dir->dir_name, name);
    for (int i = 0; i < DIR_ENT_SIZE; i++) {
        dir_ent current_entry = dir->entries[i];
        // if current_try == NULL
        if (current_entry.filename == NULL) {
            continue;
        }
        if (streq(current_entry.filename, name)) {
            printf("in directory_entry_lookup, found the entry(%s) in given dir(%s), return entry_index(%d)\n", current_entry.filename, dir->dir_name, i);
            return i;//cur_dir->entries[directory_lookup(cur_dir, "blah")]->entry_node_index;
        }
    }
    printf("in directory_entry_lookup, can't find entry(%s) in given dir(%s), return ENOENT\n", name, dir->dir_name);
    return -ENOENT; // didn't find one
}

//given a directory, name of entry,
int
directory_insert_entry(directory *dir, char *name, int inode_index) {
    int num_of_entries = dir->number_of_entries;
    printf("in directory_insert_entry, given dir(%s), entry_name(%s), inode_index(%d)\n", dir->dir_name, name, inode_index);
    if (num_of_entries >= DIR_ENT_SIZE) {
        printf("in directory_insert_entry, num_of_ent >= 32, no more room in cur_dir! return ENOSPC\n");
        return -ENOSPC;
    }
    // check if the size of the char array is > 27
    if (sizeof(*name) > FILE_NAME_LENGTH) {
        printf("in directory_insert_entry, name of entry(%s) is > 27! return ENOSPC\n", name);
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
            printf("in directory_insert_entry, found an empty spot, inserted a new entry(%s) in dir(%s), return index(%d), num_of_ent++(%d)\n",
                   new_entry.filename, dir->dir_name, i, dir->number_of_entries);
            return i;
        }
    }
    printf("in directory_insert_entry, failed to insert, reached the end of func, return ENOSPC\n");
    return -ENOSPC; // error: didn't successfully insert
}

// and inode_index of entry put an entry at an index return true if success
//delete an entry in a directory
int
directory_del_entry(directory *dir, int entry_idx) {
    int num_of_entries = dir->number_of_entries;
    if (num_of_entries <= 0) {
        printf("in directory_del_entry, given dir(%s) is empty, nothing to delete! return ENOENT\n", dir->dir_name);
        return -ENOENT;
    }
    dir_ent cur_entry = dir->entries[entry_idx];
    memset(cur_entry.filename, 0, FILE_NAME_LENGTH);
    dir->number_of_entries--;
    printf("in directory_del_entry, deleted entry at idx(%d) in dir(%s), num_of_ent--(%d), return 0\n", entry_idx, dir->dir_name, dir->number_of_entries);
    return 0;
}
