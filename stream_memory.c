#include "stream_base.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>

typedef struct {
  unsigned char* data;
  size_t offset;
  size_t length;
  bool ownsData;
} MemoryStream;

void MemoryStream_free(void *stream) {
  MemoryStream* self = stream;
  assert(self && "Cannot free a null memory stream");

  if (self->ownsData) {
    free(self->data);
  }
  free(self);
}

static int MemoryStream_read(void *stream, unsigned char *buffer, size_t offset, size_t length) {
  MemoryStream* self = stream;
  assert(self && "Cannot read from a null memory stream");

  assert(length < INT32_MAX && "Cannot copy more than 2^31 bytes (return value would overflow)");

  if ((int64_t)self->offset + (int64_t)length <= (int64_t)self->length) {
    memcpy(buffer + offset, self->data + self->offset, length);
    self->offset += length;
    return length;
  }

  int amount = self->length - self->offset;
  memcpy(buffer + offset, self->data + self->offset, amount);
  self->offset += amount;
  return amount;
}

static void MemoryStream_seek(void *stream, size_t offset) {
  MemoryStream* self = stream;
  assert(self && "Cannot seek a null memory stream");

  assert(offset <= self->length && "Cannot seek outside of memory stream");

  self->offset = offset;
}

static size_t MemoryStream_getOffset(void *stream) {
  MemoryStream* self = stream;
  assert(self && "Cannot get the offset of a null memory stream");

  return self->offset;
}

static bool MemoryStream_isEof(void *stream) {
  MemoryStream* self = stream;
  assert(self && "Cannot determine if a null memory stream is EOF");

  return self->offset >= self->length;
}

static const StreamImpl IMPL = {
  .free = &MemoryStream_free,
  .read = &MemoryStream_read,
  .seek = &MemoryStream_seek,
  .getOffset = &MemoryStream_getOffset,
  .isEof = &MemoryStream_isEof
};

Stream *Stream_new_fromMemory(unsigned char *data, size_t length, bool ownsData) {
  assert(data && "Memory stream data must not be NULL");

  Stream *base = malloc(sizeof(Stream));
  assert(base && "Failed to malloc stream");

  MemoryStream *ms = malloc(sizeof(MemoryStream));
  assert(ms && "Failed to malloc memory stream");

  ms->data = data;
  ms->offset = 0;
  ms->length = length;
  ms->ownsData = ownsData;

  base->impl = &IMPL;
  base->stream = ms;

  return base;
}
