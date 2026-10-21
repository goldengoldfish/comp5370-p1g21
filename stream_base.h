#ifndef STREAM_BASE_H
#define STREAM_BASE_H

#include <stdlib.h>
#include "stream.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  void (*free)(void*);
  int (*read)(void*, unsigned char*, size_t, size_t);
  void (*seek)(void*, size_t);
  size_t (*getOffset)(void*);
  bool (*isEof)(void*);
} StreamImpl;

typedef struct Stream {
  void* stream;
  const StreamImpl *impl;
} Stream;

#ifdef __cplusplus
}
#endif

#endif // STREAM_BASE_H
