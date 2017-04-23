#ifndef DIRECTORY_H
#define DIRECTROY_H
// created by Yifan 4/14/17



typedef struct dir_ent {
	char* filename; //Entry path
	int entry_inode_index; //Entry index
} dir_ent;

typedef struct directory {
	char* dir_name; //the maximum name length is 27 bytes, allowing entries to always have terminating 0 bytes
    dir_ent entries[32]; // Pointer to entries
    int number_of_entries;
} directory;

void directory_init(directory* cur_dir, char* name);
int directory_entry_lookup(directory* dir, char* name); //look for the inode_index of an entry
//given a directory, name of entry,
// and inode_index of entryput an entry at an index retrun true if success
int directory_insert_entry(directory* dir, char* name, int inode_index);
int directory_del_entry(directory* dir, int entry_idx); //delete an entry in a directory

#endif
