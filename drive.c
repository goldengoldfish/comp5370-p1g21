#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "drive.h"
#include "partition.h"
#include "stream.h"
#include "partition_table.h"

struct Drive {
  size_t length;
  PTable ptable;
  Stream *stream;
};

typedef struct Drive Drive;

// Constructors/destructors
Drive *Drive_new(const char *devicePath) {
  Drive *d = malloc(sizeof(Drive));
  assert(d && "Failed to malloc drive");

  d->stream = Stream_new_fromFile(devicePath);
  return d;
}

void Drive_free(Drive *self) {
  Stream_free(self->stream);
}

// Get partitions
Partition *Drive_getPartition(Drive *self, size_t index) {
  assert(0 && "todo");
}

size_t Drive_getPartitionCount(Drive *self) {
  assert(0 && "todo");
}

