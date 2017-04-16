// created by Yifan 4/14/17


typedef struct dir_ent {
	char filename[48]; //Entry path
	int inode_index; //Entry index	
} dir_ent;

typedef struct directory {
	//int pnum; // What is this for?
	dir_ent entries[256]; // Pointer to entries
} directory;

directory *directory_init();
dir_ent* entry_from_inum(int inum);//look for entry given an index
int directory_lookup(directory dd, constchar* name); //look for the index of an entry
int tree_lookup_inum(const char* path);
// maybe have a tree look up for slist?
dir_ent* entry_from_path(const char* path); //look for entry given a path
int directory_put_ent(directory dd, const char* name, int inum); //put an entry at an index retrun true if success
int directory_del(directory dd, const char* name);//delete an entry in a directory

slist* directory_list(const char* path);//puts a path into slist format
void print_directory(directory dd);
