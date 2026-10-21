#ifndef STREAM_H
#define STREAM_H

#include <stdbool.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Stream Stream;

// Constructors/destructors
Stream *Stream_new_fromFile(const char *path, size_t offset);
Stream *Stream_new_fromMemory(const unsigned char *data, size_t length);
void Stream_free(Stream *self);

// Read data
int Stream_read(Stream *self, unsigned char *buffer, size_t offset, size_t length);
void Stream_seek(Stream *self, size_t offset);
bool Stream_isEof(Stream *self);

#ifdef __cplusplus
}
#endif

#endif // STREAM_H
