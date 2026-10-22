#include "file.h"
#include <assert.h>
#include <stdlib.h>

static const char *ROOT_NAME = "<root>";

typedef struct FileNode {
  FileNodeKind kind;
  const char *name;
  FileNode **children;
  size_t numChildren;
  Stream *data;
  FileNode *parent;
  bool ownsStream;
  size_t size;
} FileNode;


FileNode *FileNode_new_root(FileNode **children, size_t numChildren) {
  if (numChildren > 0) {
    assert(children && "children must not be NULL if numChildren > 0");
  }

  FileNode *n = malloc(sizeof(FileNode));
  assert(n && "Failed to malloc file node");

  n->kind = FNK_ROOT;
  n->name = ROOT_NAME;
  n->children = children;
  n->numChildren = numChildren;
  n->data = NULL;
  n->parent = NULL;
  n->ownsStream = false;
  n->size = 0;

  for (size_t i = 0; i < n->numChildren; i++) {
    n->children[i]->parent = n;
  }
  return n;
}

FileNode *FileNode_new_file(const char *name, FileNode *parent, Stream *stream, bool ownsStream, size_t size) {
  assert(name && "Name must not be NULL");
  if (parent) {
    assert((parent->kind == FNK_ROOT || parent->kind == FNK_DIRECTORY) && "Parent must be a root or directory node");
  }
  assert(stream && "Stream must not be NULL");

  FileNode *n = malloc(sizeof(FileNode));
  assert(n && "Failed to malloc file node");

  n->kind = FNK_FILE;
  n->name = name;
  n->children = NULL;
  n->numChildren = 0;
  n->data = stream;
  n->parent = parent;
  n->ownsStream = ownsStream;
  n->size = size;
  return n;
}

FileNode *FileNode_new_directory(const char *name, FileNode *parent, FileNode **children, size_t numChildren) {
  assert(name && "Name must not be NULL");
  if (parent) {
    assert((parent->kind == FNK_ROOT || parent->kind == FNK_DIRECTORY) && "Parent must be a root or directory node");
  }
  if (numChildren > 0) {
    assert(children && "children must not be NULL if numChildren > 0");
  }

  FileNode *n = malloc(sizeof(FileNode));
  assert(n && "Failed to malloc file node");

  n->kind = FNK_DIRECTORY;
  n->name = name;
  n->children = children;
  n->numChildren = numChildren;
  n->data = NULL;
  n->parent = parent;
  n->ownsStream = false;
  n->size = 0;
  return n;
}

void FileNode_free(FileNode *self, bool deep) {
  assert(self && "Cannot free null file node");

  if (self->children) {
    for (size_t i = 0; i < self->numChildren; i++) {
      self->children[i]->parent = NULL;
      if (deep) {
        FileNode_free(self->children[i], true);
      }
    }
    free(self->children);
  }

  if (self->ownsStream) {
    free(self->data);
  }

  free(self);
}

FileNodeKind FileNode_getKind(FileNode *self) {
  return self->kind;
}

const char* FileNode_getName(FileNode *self) {
  return self->name;
}

FileNode *FileNode_getChild(FileNode *self, int index) {
  assert((self->kind == FNK_ROOT || self->kind == FNK_DIRECTORY) && "Can only get children of root or directory file nodes");
  assert(index >= 0 && (int64_t)index < (int64_t)self->numChildren && "File node child index out of bounds");
  return self->children[index];
}

size_t FileNode_getChildCount(FileNode *self) {
  assert((self->kind == FNK_ROOT || self->kind == FNK_DIRECTORY) && "Can only get children of root or directory file nodes");
  return self->numChildren;
}

FileNode *FileNode_getParent(FileNode *self) {
  return self->parent;
}

Stream *FileNode_open(FileNode *self) {
  assert(self->kind == FNK_FILE && "Can only open files for reading");
  return self->data;
}

size_t FileNode_getSize(FileNode *self) {
  assert(self->kind == FNK_FILE && "Can only get the size of a file");
  return self->size;
}

void FileNode_setChildren(FileNode *self, FileNode **children, size_t numChildren) {
  assert((self->kind == FNK_ROOT || self->kind == FNK_DIRECTORY) && "Can set the children of root or directory file nodes");

  if (self->children) {
    for (size_t i = 0; i < self->numChildren; i++) {
      FileNode_free(self->children[i], true);
    }
    free(self->children);
  }

  self->children = children;
  self->numChildren = numChildren;
}
