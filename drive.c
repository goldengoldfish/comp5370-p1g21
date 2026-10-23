#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "drive.h"
#include "partition.h"
#include "stream.h"
#include "partition_table.h"

typedef struct Drive {
  size_t length;
  PTable ptable;
  Stream *stream;
} Drive;

// Constructors/destructors
Drive *Drive_new(const char *devicePath) {
  Drive *self = malloc(sizeof(Drive));
  assert(self && "Failed to malloc drive");

  self->stream = Stream_new_fromFile(devicePath);
  self->ptable = PTable_getTable(self->stream);
  return self;
}

void Drive_free(Drive *self) {
  assert(self && "Cannot free a null drive");

  Stream_free(self->stream);
  PTable_free(&self->ptable);
  free(self);
}

// Get partitions
Partition *Drive_getPartition(Drive *self, size_t index) {
  assert(self && "Cannot get a partition from a null drive");

  assert(index < self->ptable.numPartitions && "Partition index out of bounds");

  return self->ptable.partitionList[index];
}

size_t Drive_getPartitionCount(Drive *self) {
  assert(self && "Cannot get the partition count of a null drive");
  return self->ptable.numPartitions;
}

Stream *Drive_getImage(Drive *self) {
  assert(self && "Cannot get the image of a null drive");
  return self->stream;
}
