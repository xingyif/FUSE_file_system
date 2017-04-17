// created by Yifan 4/14/17


#include "slist.h"

typedef struct dir_ent {
	char* filename; //Entry path
	int entry_inode_index; //Entry index
} dir_ent;

typedef struct directory {
	char* dir_name; //the maximum name length is 27 bytes, allowing entries to always have terminating 0 bytes
    dir_ent* entries[32]; // Pointer to entries
    int number_of_entries;
} directory;

directory* directory_init(char* name);
int directory_lookup(directory* dir, char* name); //look for the inode_index of an entry
//int tree_lookup_inum(const char* path);
// maybe have a tree look up for slist?
int directory_insert_entry(directory* dir, char* name, int inode_index); //given a directory, name of entry,
// and inode_index of entryput an entry at an index retrun true if success
int directory_del_entry(directory* dir, char* name); //delete an entry in a directory

//slist* directory_list(const char* path);// for the tree: puts a path into slist format
void print_directory(directory dd);
