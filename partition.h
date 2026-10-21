#ifndef PARTITION_H
#define PARTITION_H

#include <stdlib.h>
#include "file.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PK_UNKNOWN = 0,
    PK_FAT16 = 1,
    // etc.
} PartitionKind;

typedef struct Partition Partition;

// Basic getters
PartitionKind Partition_getKind(Partition *self);


// Get position (on the drive)
size_t Partition_getOffset(Partition *self);
size_t Partition_getLength(Partition *self);

// Constructors/destructors
Partition *Partition_new_fat16(Stream *data);
// etc.
void Partition_free(Partition *self);

// File management
FileNode *Partition_getRoot(Partition *self);

#ifdef __cplusplus
}
#endif

#endif // PARTITION_H
