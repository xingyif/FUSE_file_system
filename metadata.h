// created by Yifan 04/15/27
// modified by brett & yifan


typedef struct metadata {
	char[3] mode; // r/w/x
	int user_id; // uint16_t
	int size_of; // size of file/dir in bytes
	int is_file; // flag, if obj is file = 1, dir = 0
} metadata;

