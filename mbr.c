
#include <stdbool.h>
#include <stdint.h>
#include "mbr.h"
#include "stream.h"
#include "assert.h"
/* Read a little-endian 32-bit value from four bytes. */
static uint32_t read_le32(const unsigned char *bytes) {
    return (uint32_t)bytes[0]
         | ((uint32_t)bytes[1] << 8)
         | ((uint32_t)bytes[2] << 16)
         | ((uint32_t)bytes[3] << 24);
}

// check for MBR file signature without saving to MBRInfo buffer.
int check_mbr(Stream *image) {
    return read_mbr(image, NULL);
}

int read_mbr(Stream *image, MBRInfo *mbr) {
    assert(image && "Cannot read image");
    // assert(mbr && "mbr cannot be NULL");

    unsigned char buffer[MBR_SIZE];
    Stream *reader = Stream_new_fromMemory(buffer, MBR_SIZE, 0);

    // covered by asserts at beginning
    // if (image == NULL || mbr == NULL) {
    //     return 1;
    // }

    // covered by assert in Stream_new_fromMemory();
    // /* Start reading at the beginning of the disk image. */
    // if (fseek(image, 0, SEEK_SET) != 0) {
    //     return 2;
    // }

    /* An MBR requires at least 512 bytes. */
    if (Stream_read(reader, buffer, 1, MBR_SIZE) != MBR_SIZE) {
        assert(0 && "Cannot read MBR");
        return 3;
    }
    Stream_free(reader);

    /* Check the MBR signature at offsets 510 and 511. */
    if (buffer[510] != 0x55 || buffer[511] != 0xAA) {
        // assert(0 && "Image does not match MBR signature");
        return 4;
    }

    // mbr most likely null if just checking for MBR sig
    if (!mbr) {
        return 0;
    }

    /* Each partition entry is 16 bytes, starting at offset 446. */
    mbr->numPartitions = MBR_PARTITION_COUNT;
    for (int i = 0; i < MBR_PARTITION_COUNT; i++) {
        const unsigned char *entry = &buffer[446 + i * 16];

        mbr->partitions[i].boot_flag = entry[0];
        mbr->partitions[i].partition_type = entry[4];
        mbr->partitions[i].start_sector = read_le32(&entry[8]);
        mbr->partitions[i].sector_count = read_le32(&entry[12]);
    }

    return 0;
}
