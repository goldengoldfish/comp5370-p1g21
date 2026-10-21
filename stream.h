#ifndef STREAM_H
#define STREAM_H

typedef void* Stream;

// Constructors/destructors
Stream Stream_new_fromFile(const char *path, size_t offset);
Stream Stream_new_fromMemory(const unsigned char *data, size_t length);
void Stream_free(Stream this);

// Read data
int Stream_read(Stream this, unsigned char *buffer, size_t offset, size_t length);
void Stream_seek(Stream this, size_t offset);
bool Stream_isEof(Stream this);

#endif // STREAM_H
