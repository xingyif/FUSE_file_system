# FUSE_File_System

Wait...till I finish

80% there
.
.
.


>  This file system is a **FUSE filesystem driver** that will let you mount a **1MB** disk image (data file) as a filesystem.
>  This file system uses **FUSE** library and tools. Runnable on **Ubuntu 16.04 system** (which has FUSE version 2.6).

**Some basic functionalities of the file system:**
* Create files.
* List the files in the filesystem root directory (where you mounted it).
* Write to small files (under 4k).
* Read from small files (under 4k).
* Delete files.
* Rename files.

**Additional functionalities:**
* Create hard links - multiple names for the same file blocks.
* Read and write from files larger than one block. For example, you should be able to support fifty 1k files or five 100k * files.
* Create directories and nested directories. Directory depth should only be limited by space.
* Remove directories.
* Support metadata (permissions and timestamps) for files and directories.
