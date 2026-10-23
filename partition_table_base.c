#include "mbr.h"
#include "partition.h"
#include "partition_table.h"
#include "stream.h"
#include <assert.h>
#include <string.h>

// *Ideally* this will be written in an extensible way to eventually support
// GPT, but I'm doing just MBR for now.

PTable PTable_getTable(Stream *stream) {
  MBRInfo mbr;
	assert(!read_mbr(stream, &mbr) && "Failed to read MBR");

  PTable result = {
    .pTableKind = PTK_MBR,
    .numPartitions = mbr.numPartitions,
  };

  memset(&result.partitionList, 0, sizeof(result.partitionList));
  for (size_t i = 0; i < result.numPartitions; i++) {
    result.partitionList[i] = mbr.partitions[i].partition;
  }

  return result;
}

void PTable_free(PTable *self) {
  for (size_t i = 0; i < self->numPartitions; i++) {
    Partition_free(self->partitionList[i]);
  }
}
