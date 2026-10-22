#include "stream_base.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
  FILE *handle;
  size_t offset;
} FileStream;

static void FileStream_free(void *stream) {
  FileStream *self = stream;
  assert(self && "Cannot free a null file stream");

  fclose(self->handle);
  free(self);
}

static int FileStream_read(void *stream, unsigned char *buffer, size_t offset, size_t length) {
  FileStream *self = stream;
  assert(self && "Cannot read from a null file stream");

  assert(length < INT32_MAX && "Cannot copy more than 2^31 bytes (return value would overflow)");

  if (feof(self->handle)) {
    return 0;
  }

  int read = fread(buffer + offset, 1, length, self->handle);
  assert(!ferror(self->handle) && "Error when reading file stream");

  self->offset += read;
  return read;
}

static void FileStream_seek(void *stream, size_t offset) {
  FileStream *self = stream;
  assert(self && "Cannot seek a null file stream");

  assert(!fseek(self->handle, offset, SEEK_SET) && "Failed to seek file stream");
  self->offset = offset;
}

size_t FileStream_getOffset(void *stream) {
  FileStream *self = stream;
  assert(self && "Cannot get the offset of a null file stream");

  return self->offset;
}

bool FileStream_isEof(void *stream) {
  FileStream *self = stream;
  assert(self && "Cannot determine if a null file stream is EOF");

  return feof(self->handle);
}

static const StreamImpl IMPL = {
  .free = &FileStream_free,
  .read = &FileStream_read,
  .seek = &FileStream_seek,
  .getOffset = &FileStream_getOffset,
  .isEof = &FileStream_isEof
};

Stream *Stream_new_fromFile(const char *path) {
  Stream *base = malloc(sizeof(Stream));
  assert(base && "Failed to malloc stream");

  FileStream *fs = malloc(sizeof(FileStream));
  assert(fs && "Failed to malloc file stream");

  fs->handle = fopen(path, "rb");
  assert(fs->handle && "Failed to open file stream");

  base->stream = fs;
  base->impl = &IMPL;

  return base;
}
