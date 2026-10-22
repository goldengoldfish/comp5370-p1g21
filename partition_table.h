#ifndef PARTITION_TABLE_H
#define PARTITION_TABLE_H

#include "partition.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  UNKNOWN = 0,
  MBR,
  GPT
} PTableKind;

typedef struct {
  PTableKind pTableKind;
  size_t numPartitions;
  Partition *partitionList[4];
} PTable;

// Identify and read info from partition table from stream
PTable PTable_getTable(Stream *stream);

void PTable_free(PTable *self);

#ifdef __cplusplus
}
#endif

#endif
