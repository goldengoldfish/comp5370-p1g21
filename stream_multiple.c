#include "stream_base.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  Stream **others;
  size_t *lengths;
  size_t numOthers;
  bool ownsOthers;
  size_t readOffset;
} MultiStream;

static void MultiStream_free(void *stream) {
  MultiStream *self = stream;
  assert(self && "Cannot free a null multi stream");

  if (self->ownsOthers) {
    for (size_t i = 0; i < self->numOthers; i++) {
      Stream_free(self->others[i]);
    }
    free(self->others);
    free(self->lengths);
  }

  free(self);
}

static size_t min(size_t a, size_t b) {
  return a < b ? a : b;
}

static int MultiStream_read(void *stream, unsigned char *buffer, size_t offset, size_t length) {
  MultiStream *self = stream;
  assert(self && "Cannot read from a null multi stream");

  size_t localReadOffset = self->readOffset;
  size_t index;
  for(index = 0; index < self->numOthers; index++) {
    if (localReadOffset < self->lengths[index]) {
      break;
    }

    localReadOffset -= self->lengths[index];
  }

  int totalRead = 0;
  while (index < self->numOthers) {
    // Read as much as we can from this stream
    size_t toRead = min(length - totalRead, self->lengths[index] - localReadOffset);
    int read = Stream_read(self->others[index], buffer, offset + totalRead, toRead);

    totalRead += read;
    localReadOffset += read;
    self->readOffset += read;

    // If we did NOT hit EOF, we are done
    if (localReadOffset < self->lengths[index]) {
      break;
    }

    // Move to next stream
    localReadOffset -= self->lengths[index];
    index++;
  }

  return totalRead;
}

void MultiStream_seek(void *stream, size_t offset) {
  MultiStream *self = stream;
  assert(self && "Cannot seek a null multi stream");

  bool foundPartiallyReadStream = false;
  for (size_t i = 0; i < self->numOthers; i++) {
    if (offset > self->lengths[i]) { // We have read past this stream; seek to EOF
      Stream_seek(self->others[i], self->lengths[i]);
      offset -= self->lengths[i];
    } else if (!foundPartiallyReadStream) { // This is the first stream that we haven't fully read; seek to offset within
      Stream_seek(self->others[i], offset);
      foundPartiallyReadStream = true;
    } else { // We have not gotten to this stream yet; seek to start
      Stream_seek(self->others[i], 0);
    }
  }
}

size_t MultiStream_getOffset(void *stream) {
  MultiStream *self = stream;
  assert(self && "Cannot get the offset of a null multi stream");

  return self->readOffset;
}

bool MultiStream_isEof(void* stream) {
  MultiStream *self = stream;
  assert(self && "Cannot determine if a null multi stream is EOF");

  size_t totalLength = 0;
  for (size_t i = 0; i < self->numOthers; i++) {
    totalLength += self->lengths[i];
  }

  return self->readOffset >= totalLength;
}

static const StreamImpl IMPL = {
  .free = &MultiStream_free,
  .read = &MultiStream_read,
  .seek = &MultiStream_seek,
  .getOffset = &MultiStream_getOffset,
  .isEof = &MultiStream_isEof
};

Stream *Stream_new_fromMultiple(Stream **others, size_t *lengths, size_t numOthers, bool ownsOthers) {
  Stream *self = malloc(sizeof(Stream));
  assert(self && "Failed to malloc stream");

  MultiStream *m = malloc(sizeof(MultiStream));
  assert(m && "Failed to malloc multi stream");

  m->others = others;
  m->numOthers = numOthers;
  m->ownsOthers = ownsOthers;
  m->lengths = lengths;
  self->impl = &IMPL;
  self->stream = m;

  return self;
}
