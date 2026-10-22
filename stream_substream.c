#include "stream_base.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
  Stream *parent;
  size_t startOffset;
  size_t readOffset;
  size_t length;
} Substream;

void Substream_free(void *stream) {
  Substream *self = stream;
  assert(self && "Cannot free a null substream");

  free(self);
}

static size_t min(size_t a, size_t b) {
  return a < b ? a : b;
}

int Substream_read(void *stream, unsigned char *buffer, size_t offset, size_t length) {
  Substream *self = stream;
  assert(self && "Cannot read from a null substream");

  assert(length < INT32_MAX && "Cannot copy more than 2^31 bytes (return value would overflow)");

  size_t prevOffset = Stream_getOffset(self->parent);
  Stream_seek(self->parent, self->startOffset + self->readOffset);

  size_t toRead = min(length, self->length - self->readOffset);
  int amount = Stream_read(self->parent, buffer, offset, toRead);
  self->readOffset += amount;

  Stream_seek(self->parent, prevOffset);
  return amount;
}

void Substream_seek(void *stream, size_t offset) {
  Substream *self = stream;
  assert(self && "Cannot seek a null substream");

  assert(offset <= self->length && "Cannot seek outside of substream");

  self->readOffset = offset;
}

size_t Substream_getOffset(void *stream) {
  Substream *self = stream;
  assert(self && "Cannot get the offset of a null substream");

  return self->readOffset;
}

bool Substream_isEof(void *stream) {
  Substream *self = stream;
  assert(self && "Cannot determine if a null substream is EOF");

  return self->readOffset >= self->length;
}

static const StreamImpl IMPL = {
  .free = &Substream_free,
  .read = &Substream_read,
  .seek = &Substream_seek,
  .getOffset = &Substream_getOffset,
  .isEof = &Substream_isEof
};

Stream *Stream_new_fromStream(Stream *parent, size_t offset, size_t length) {
  assert(parent && "Parent stream must be not null");

  Stream *base = malloc(sizeof(Stream));
  assert(base && "Failed to malloc stream");

  Substream *ss = malloc(sizeof(Substream));
  assert(ss && "Failed to malloc substream");

  base->impl = &IMPL;
  base->stream = ss;

  ss->parent = parent;
  ss->startOffset = offset;
  ss->readOffset = 0;
  ss->length = length;

  return base;
}
