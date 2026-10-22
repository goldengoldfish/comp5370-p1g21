#ifndef STREAM_H
#define STREAM_H

#include <stdbool.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Stream Stream;

// Constructors/destructors
Stream *Stream_new_fromFile(const char *path);
Stream *Stream_new_fromStream(Stream *parent, size_t offset, size_t length);
Stream *Stream_new_fromMemory(unsigned char *data, size_t length, bool ownsData);
Stream *Stream_new_fromMultiple(Stream **others, size_t *lengths, size_t numOthers, bool ownsOthers);
void Stream_free(Stream *self);

// Read data
// Note that `offset` refers to the offset within the output buffer the data is copied to
int Stream_read(Stream *self, unsigned char *buffer, size_t offset, size_t length);
void Stream_seek(Stream *self, size_t offset);

// Basic getters
size_t Stream_getOffset(Stream *self);
bool Stream_isEof(Stream *self);

#ifdef __cplusplus
}
#endif

#endif // STREAM_H
