#ifndef PARTITION_BASE_H
#define PARTITION_BASE_H

#include "partition.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  void (*free)(void*);
  size_t (*getOffset)(void*);
  size_t (*getLength)(void*);
  FileNode* (*getRoot)(void*);
} PartitionImpl;

typedef struct Partition {
  void* partition;
  PartitionKind kind;
  const PartitionImpl *impl;
} Partition;

#ifdef __cplusplus
}
#endif

#endif // PARTITION_BASE_H
