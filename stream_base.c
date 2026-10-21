#include "stream_base.h"

void Stream_free(Stream *self) {
  self->impl->free(self->stream);
  free(self);
}

int Stream_read(Stream *self, unsigned char *buffer, size_t offset, size_t length) {
  return self->impl->read(self->stream, buffer, offset, length);
}

void Stream_seek(Stream *self, size_t offset) {
  self->impl->seek(self->stream, offset);
}

size_t Stream_getOffset(Stream *self) {
  return self->impl->getOffset(self->stream);
}

bool Stream_isEof(Stream *self) {
  return self->impl->isEof(self->stream);
}
