#ifndef DRIVE_H
#define DRIVE_H

typedef struct void* Drive;

// Constructors/destructors
Drive Drive_new(const char *devicePath);
void Drive_free(Drive this);

// Get partitions
Partition Drive_getPartition(Drive this, size_t index);
size_t Drive_getPartitionCount(Drive this);

#endif // DRIVE_H
