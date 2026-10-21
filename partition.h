#ifndef PARTITION_H
#define PARTITION_H

typedef enum {
    PK_UNKNOWN = 0,
    PK_FAT16 = 1,
    // etc.
} PartitionKind;

typedef void* Partition;

// Basic getters
PartitionKind Partition_getKind(Partition this);

// Get position (on the drive)
size_t Partition_getOffset(Partition this);
size_t Partition_getLength(Partition this);

// Constructors/destructors
Partition Partition_new_fat16(Stream data);
// etc.
void Partition_free(Partition this);

// File management
FileNode Partition_getRoot(Partition this);

#endif // PARTITION_H
