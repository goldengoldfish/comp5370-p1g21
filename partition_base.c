#include "partition_base.h"
#include <assert.h>

PartitionKind Partition_getKind(Partition *self) {
  return self->kind;
}

void Partition_free(Partition *self) {
  assert(self && "cannot free a null stream");

  self->impl->free(self->partition);
  free(self);
}

size_t Partition_getOffset(Partition *self) {
  return self->impl->getOffset(self->partition);
}

size_t Partition_getLength(Partition *self) {
  return self->impl->getLength(self->partition);
}

FileNode *Partition_getRoot(Partition *self) {
  return self->impl->getRoot(self->partition);
}
