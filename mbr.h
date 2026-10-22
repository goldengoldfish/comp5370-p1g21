
#ifndef MBR_H
#define MBR_H

#include <stdint.h>
#include <stdio.h>
#include "partition.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MBR_SIZE 512
#define MBR_PARTITION_COUNT 4

typedef struct {
    uint8_t boot_flag;
    uint8_t partition_type;
    uint32_t start_sector;
    uint32_t sector_count;
    Partition *partition;
} MBRPartition;

typedef struct {
    size_t numPartitions;
    MBRPartition partitions[MBR_PARTITION_COUNT];
} MBRInfo;

/*
 * Reads and validates an MBR from an open disk image.
 * Returns 0 on success and a nonzero value on failure.
 */
int read_mbr(Stream *image, MBRInfo *mbr);

#ifdef __cplusplus
}
#endif

#endif
