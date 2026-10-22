#ifndef FILE_H
#define FILE_H

#include <stdlib.h>
#include <stdbool.h>
#include "stream.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FNK_UNKNOWN = 0,
    FNK_ROOT = 1,
    FNK_FILE = 2,
    FNK_DIRECTORY = 3
} FileNodeKind;

typedef struct FileNode FileNode;

// Constructors/destructors
FileNode *FileNode_new_root(const char *name, FileNode **children, size_t numChildren);
FileNode *FileNode_new_file(const char *name, FileNode *parent, Stream *stream, bool ownsStream, size_t size);
FileNode *FileNode_new_directory(const char *name, FileNode *parent, FileNode **children, size_t numChildren);
// Use `deep` to free subtree as well
// This will zero each child's reference to self node as well (i.e., getParent() will return NULL)
void FileNode_free(FileNode *self, bool deep); 

// Basic getters
FileNodeKind FileNode_getKind(FileNode *self);
const char* FileNode_getName(FileNode *self);

// Traverse directories
FileNode *FileNode_getChild(FileNode *self, int index);
size_t FileNode_getChildCount(FileNode *self);
FileNode *FileNode_getParent(FileNode *self);

// Read files
Stream *FileNode_open(FileNode *self);
size_t FileNode_getSize(FileNode *self);

// Set directory contents
void FileNode_setChildren(FileNode *self, FileNode **children, size_t numChildren);

#ifdef __cplusplus
}
#endif

#endif // FILE_H
