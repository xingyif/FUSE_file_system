// Created by Brett

#include <stdlib.h>
#include <stdio.h>
#include "directory.h"
#include "slist.h"
const int FILE_NAME_LENGTH  = 27;
const int DIR_ENT_SIZE  = 32;

void
directory_init(directory* cur_dir, char* name) {
//	directory* dir = malloc(sizeof(directory));
    // todo check if the size of the char array is > 27
    if (sizeof(*name) > FILE_NAME_LENGTH) {
        perror("Name of file/dir is too long!");
    }
    cur_dir->dir_name = name;
    cur_dir->number_of_entries = 0;
    // entries are fixed size array
}

// look for the entry index of a name inside a dir
int
directory_entry_lookup(directory* dir, char* name) {
    int num_of_entries = dir->number_of_entries;
    for (int i = 0; i < DIR_ENT_SIZE; i++) {
        dir_ent* current_entry = dir->entries[i];
        if (current_entry == NULL) {
            continue;
        }
        if (current_entry->filename == name) {
            return i;//cur_dir->entries[directory_lookup(cur_dir, "blah")]->entry_node_index;
        }
    }
    return -1; // didn't find one
}

//given a directory, name of entry,
int
directory_insert_entry(directory* dir, char* name, int inode_index) {
    int num_of_entries = dir->number_of_entries;
    if (num_of_entries >= DIR_ENT_SIZE) {
        perror("No room to store more in the current directory!");
    }
    // todo check if the size of the char array is > 27
    if (sizeof(*name) > FILE_NAME_LENGTH) {
        perror("Name of file/dir is too long!");
    }
    dir_ent* new_entry = malloc(sizeof(dir_ent));
    new_entry->filename = name;
    new_entry->entry_inode_index = inode_index;

    for (int i = 0; i < num_of_entries; i++) {
        dir_ent* current_entry = dir->entries[i];
        if (current_entry == NULL) {
            dir->entries[i] = new_entry;
            dir->number_of_entries++;
            return i;
        }
    }
    return -1; // error: something went wrong
}
// and inode_index of entryput an entry at an index retrun true if success
//delete an entry in a directory
int directory_del_entry(directory* dir, char* name) {
    int num_of_entries = dir->number_of_entries;
    if (num_of_entries <= 0) {
        perror("Nothing to delete in the current directory!");
    }

    for (int i = 0; i < num_of_entries; i++) {
        dir_ent* current_entry = dir->entries[i];
        if (current_entry->filename == name) {
            dir->entries[i] = NULL;
            dir->number_of_entries--;
            free(current_entry);
            return i;
        }
    }
    return -1;
}
