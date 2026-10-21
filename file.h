#ifndef FILE_H
#define FILE_H

typedef enum {
    FN_UNKNOWN = 0,
    FN_ROOT = 1,
    FN_FILE = 2,
    FN_DIRECTORY = 3
} FileNodeType;

typedef void* FileNode;

// Constructors/destructors
FileNode FileNode_new_root(const char *name);
FileNode FileNode_new_file(const char *name, size_t offset, size_t length);
FileNode FileNode_new_directory(const char *name, FileNode *children, size_t numChildren);
// Use `deep` to free subtree as well
// This will zero each child's reference to this node as well (i.e., getParent() will return NULL)
void FileNode_free(bool deep); 

// Basic getters
FileNodeType FileNode_getType(FileNode this);
const char* FileNode_getName(FileNode this);

// Traverse directories
FileNode FileNode_getChild(FileNode this, int index);
size_t FileNode_getChildCount(FileNode this);
FileNode FileNode_getParent(FileNode this);

// Read files
size_t FileNode_getOffset(FileNode this); // Relative to the file's partition
size_t FileNode_getLength(FileNode this);
Stream FileNode_open(FileNode this);

#endif // FILE_H
