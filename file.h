#ifndef FILE_H
#define FILE_H

#include <stdlib.h>
#include "stream.h"

#ifdef __cplusplus
extern "C" {
#endif

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
// This will zero each child's reference to self node as well (i.e., getParent() will return NULL)
void FileNode_free(bool deep); 

// Basic getters
FileNodeType FileNode_getType(FileNode self);
const char* FileNode_getName(FileNode self);

// Traverse directories
FileNode FileNode_getChild(FileNode self, int index);
size_t FileNode_getChildCount(FileNode self);
FileNode FileNode_getParent(FileNode self);

// Read files
size_t FileNode_getOffset(FileNode self); // Relative to the file's partition
size_t FileNode_getLength(FileNode self);
Stream FileNode_open(FileNode self);

#ifdef __cplusplus
}
#endif

#endif // FILE_H
