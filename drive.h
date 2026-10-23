#ifndef DRIVE_H
#define DRIVE_H

#include <stdlib.h>
#include "partition.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Drive Drive;

// Constructors/destructors
Drive *Drive_new(const char *devicePath);
void Drive_free(Drive *self);

// Get partitions
Partition *Drive_getPartition(Drive *self, size_t index);
size_t Drive_getPartitionCount(Drive *self);

// Get full disk image stream
Stream *Drive_getImage(Drive *self);

#ifdef __cplusplus
}
#endif

#endif // DRIVE_H
