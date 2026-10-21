#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "drive.h"
#include "partition.h"

typedef enum {
  MBR,
  GPT
} PartitionTable;

struct Drive {
  size_t size;
  PartitionTable partition_table;
  Partition *partitions;
  size_t partition_count;
};

typedef struct Drive Drive;

// Constructors/destructors
Drive *Drive_new(const char *devicePath) {
  assert(0 && "todo");
}

void Drive_free(Drive *self) {
  assert(0 && "todo");
}

// Get partitions
Partition *Drive_getPartition(Drive *self, size_t index) {
  assert(0 && "todo");
}

size_t Drive_getPartitionCount(Drive *self) {
  assert(0 && "todo");
}

