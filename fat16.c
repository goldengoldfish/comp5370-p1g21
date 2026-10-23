#include "file.h"
#include "partition_base.h"
#include "stream.h"
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define READ_INT16_LE(buf, i) ((buf)[i] | ((buf)[(i) + 1] << 8))
#define READ_INT32_LE(buf, i) ((buf)[i] | ((buf)[(i) + 1] << 8) | ((buf)[(i) + 2] << 16) | ((buf)[(i) + 3] << 24))

typedef struct {
  FileNode *rootNode;
  size_t start, length;
} Fat16Partition;

static void Fat16Partition_free(void *partition) {
  Fat16Partition *self = partition;
  assert(self && "Cannot free null FAT16 partition");

  FileNode_free(self->rootNode, true);
  free(self);
}

static FileNode *Fat16Partition_getRoot(void *partition) {
  Fat16Partition *self = partition;
  assert(self && "Cannot get the root of a null FAT16 partition");

  return self->rootNode;
}

static size_t Fat16Partition_getOffset(void *partition) {
  Fat16Partition *self = partition;
  assert(self && "Cannot get the offset of a null FAT16 partition");

  return self->start;
}

static size_t Fat16Partition_getLength(void *partition) {
  Fat16Partition *self = partition;
  assert(self && "Cannot get the length of a null FAT16 partition");

  return self->length;
}

static PartitionImpl IMPL = {
  .free = &Fat16Partition_free,
  .getRoot = &Fat16Partition_getRoot,
  .getOffset = &Fat16Partition_getOffset,
  .getLength = &Fat16Partition_getLength,
};


#define VOLUME_LABEL_MAX_LENGTH 11

typedef struct {
  uint16_t sectorSize;
  uint8_t sectorsPerCluster;
  uint16_t reservedSectors;
  uint8_t fatCount;
  uint16_t rootEntryCount;
  uint16_t sectorsPerFat;
  char name[VOLUME_LABEL_MAX_LENGTH];
} Fat16BootRecord;

static Fat16BootRecord readBootRecord(Stream *data) {
  unsigned char buf[512];
  assert(Stream_read(data, buf, 0, 512) == 512 &&
         "Unexpected EOF when reading FAT Boot Record");

  assert((buf[0x26] == 0x28 || buf[0x26] == 0x29) && "Missing FAT16 signature");

  // Read BIOS parameter block
  Fat16BootRecord bootRecord = {
    .sectorSize = READ_INT16_LE(buf, 0xB),
    .sectorsPerCluster = buf[0xD],
    .reservedSectors = READ_INT16_LE(buf, 0xE),
    .fatCount = buf[0x10],
    .rootEntryCount = READ_INT16_LE(buf, 0x11),
    .sectorsPerFat = READ_INT16_LE(buf, 0x16)
  };


  // Find end of label (trim spaces)
  int labelLength = VOLUME_LABEL_MAX_LENGTH;
  for (int i = VOLUME_LABEL_MAX_LENGTH - 1; i >= 0; i--) {
    if (buf[0x2B + i] != ' ') {
      labelLength = i + 1;
      break;
    }
  }

  memset(bootRecord.name, '\0', VOLUME_LABEL_MAX_LENGTH);
  memcpy(bootRecord.name, &buf[0x2B], labelLength);

  return bootRecord;
}

typedef enum {
  FEK_FILE,
  FEK_DIRECTORY,
  FEK_VOLUME
} FatEntryKind;

typedef struct {
  FatEntryKind kind;
  const char *name;
  uint32_t cluster;
  uint32_t size;
} FatEntry;

#define CHARS_PER_PART 13
typedef struct {
  uint8_t order;
  uint16_t chars[CHARS_PER_PART];
} LFNPart;

static int compareLFNParts(const void *a, const void* b) {
  const LFNPart *pa = a;
  const LFNPart *pb = b;
  return (pa->order & 0x1F) - (pb->order & 0x1F);
}

bool parseEntry(Stream *data, FatEntry *out) {
  unsigned char entry[32];
  while (true) {
    assert(Stream_read(data, entry, 0, 32) == 32 && "Failed to read FAT entry");

    // No more entries
    if (!entry[0]) {
      return false;
    }

    if (entry[0] == 0xE5) {
      continue;
    }

    break;
  }

  char *name = NULL;

  // LFN name
  if (entry[11] == 0xF) {
    LFNPart* parts = malloc(sizeof(LFNPart));
    assert(parts && "Failed to alloc buffer for LNF parsing");

    // Extract all LFN entries
    size_t count = 0;
    size_t capacity = 1;
    do {
      if (count >= capacity) {
        capacity *= 2;
        parts = realloc(parts, capacity * sizeof(LFNPart));
        assert(parts && "Failed to realloc buffer for LNF parsing");
      }
      
      parts[count].order = entry[0];
      memcpy(&parts[count].chars[0], &entry[1], 10);
      memcpy(&parts[count].chars[5], &entry[14], 12);
      memcpy(&parts[count].chars[11], &entry[28], 4);

      assert(Stream_read(data, entry, 0, 32) == 32 && "Failed to read FAT entry");
      count++;
    } while (entry[11] == 0xF);

    // Sort by order byte
    qsort(parts, count, sizeof(LFNPart), &compareLFNParts);

    name = calloc((count * CHARS_PER_PART) + 1, 1);

    // Reconstruct full string
    size_t p = 0;
    for (size_t i = 0; i < count; i++) {
      for (size_t j = 0; j < CHARS_PER_PART; j++) {
        char c = parts[i].chars[j];
        assert(!(c & 0x80) && "UTF-16 in LFN not supported");
        if (c == 0) {
          i = count;
          break;
        }
        name[p++] = c;
      }
    }

    free(parts);
  }

  assert(entry[11] != 0xF && "We should be past any LFN blocks!");
  assert(entry[0] != 0 && "Ran out of entries!");

  // Non-LFN name
  if (!name) {
    name = calloc(12, 1); // 8 name + . + 3 extension
    int nameEnd = 7;
    while (nameEnd >= 0 && entry[nameEnd] == ' ') {
      nameEnd--;
    }
    memcpy(name, entry, nameEnd + 1);

    int extEnd = 10;
    while (extEnd >= 8 && entry[extEnd] == ' ') {
      extEnd--;
    }

    if (extEnd >= 8) {
      name[nameEnd + 1] = '.';
      memcpy(name + nameEnd + 2, entry + 8, extEnd - 8 + 1);
    }
  }

  out->name = name;

  if (entry[11] & 0x08) {
    out->kind = FEK_VOLUME;
  } else if (entry[11] & 0x10) {
    out->kind = FEK_DIRECTORY;
  } else {
    out->kind = FEK_FILE;
  }

  out->cluster = READ_INT16_LE(entry, 26) | (READ_INT16_LE(entry, 20) << 16);
  out->size = READ_INT32_LE(entry, 28);
  return true;
}

static size_t min(size_t a, size_t b) {
  return a < b ? a : b;
}

Stream *createMultiStreamFromClusters(Stream *fat, Stream *partition, uint32_t cluster, Fat16BootRecord bootRecord, size_t maxLength) {
  Stream **sectors = malloc(sizeof(Stream*));
  size_t *lengths = malloc(sizeof(size_t));
  size_t sectorCount = 0;
  size_t capacity = 1;

  unsigned char buf[2];
  while (cluster < 0xFFF8) {
    sectorCount++;

    size_t sectorOffset = (cluster - 2) * bootRecord.sectorsPerCluster;

    if (sectorCount >= capacity) {
      capacity *= 2;
      sectors = realloc(sectors, sizeof(Stream*) * capacity);
      lengths = realloc(lengths, sizeof(size_t) * capacity);
    }

    sectors[sectorCount - 1] = Stream_new_fromStream(partition, sectorOffset * bootRecord.sectorSize, bootRecord.sectorSize * bootRecord.sectorsPerCluster);

    lengths[sectorCount - 1] = min(bootRecord.sectorSize, maxLength);
    maxLength -= lengths[sectorCount - 1];

    Stream_seek(fat, cluster * 2);
    Stream_read(fat, buf, 0, 2);
    cluster = READ_INT16_LE(buf, 0);
  }

  return Stream_new_fromMultiple(sectors, lengths, sectorCount, true);
}

void traverseDirectory(Stream *fat, Stream *root, Stream *data, FileNode *parent, FileNode ***children, size_t *numChildren, Fat16BootRecord bootRecord) {
  *children = NULL;
  *numChildren = 0;

  FatEntry e;
  while (parseEntry(root, &e)) {
    switch (e.kind) {
      case FEK_FILE:
        (*numChildren)++;
        *children = realloc(*children, sizeof(FileNode*) * (*numChildren));
        (*children)[(*numChildren) - 1] = FileNode_new_file(e.name, parent, createMultiStreamFromClusters(fat, data, e.cluster, bootRecord, e.size), true, e.size);
        break;
      case FEK_DIRECTORY:
        if (strcmp(".", e.name) == 0 || strcmp("..", e.name) == 0) {
          continue;
        }

        FileNode **dirChildren;
        size_t dirChildCount = 0;

        (*numChildren)++;
        *children = realloc(*children, sizeof(FileNode*) * (*numChildren));
        FileNode *n = FileNode_new_directory(e.name, parent, NULL, 0);

        Stream *dirRoot = createMultiStreamFromClusters(fat, data, e.cluster, bootRecord, SIZE_MAX);
        traverseDirectory(fat, dirRoot, data, n, &dirChildren, &dirChildCount, bootRecord);

        FileNode_setChildren(n, dirChildren, dirChildCount);

        (*children)[(*numChildren) - 1] = n;
        break;
      default:
        continue;
    }
  }

}

Partition *Partition_new_fat16(Stream *partition, size_t start, size_t length) {
  Partition *base = malloc(sizeof(Partition));
  assert(base && "Could not malloc partition");

  Fat16Partition *fat16 = malloc(sizeof(Fat16Partition));
  assert(fat16 && "Could not malloc FAT16 partition");

  base->kind = PK_FAT16;
  base->partition = fat16;
  base->impl = &IMPL;

  size_t startOffset = Stream_getOffset(partition);

  Fat16BootRecord bootRecord = readBootRecord(partition);

  int rootSectors = ((bootRecord.rootEntryCount * 32) + (bootRecord.sectorSize - 1)) / bootRecord.sectorSize;
  int firstRootSector = (bootRecord.reservedSectors + (bootRecord.fatCount * bootRecord.sectorsPerFat));
  int firstDataSector = firstRootSector + rootSectors;

  Stream *fat = Stream_new_fromStream(partition, startOffset + (bootRecord.sectorSize * bootRecord.reservedSectors), bootRecord.sectorSize * bootRecord.sectorsPerFat);
  Stream *root = Stream_new_fromStream(partition, startOffset + (bootRecord.sectorSize * firstRootSector), bootRecord.sectorSize * rootSectors);
  Stream *data = Stream_new_fromStream(partition, startOffset + (bootRecord.sectorSize * firstDataSector), length);

  FileNode **children;
  size_t numChildren;
  traverseDirectory(fat, root, data, NULL, &children, &numChildren, bootRecord);

  fat16->rootNode = FileNode_new_root(bootRecord.name[0] ? bootRecord.name : NULL, children, numChildren);
  fat16->start = start;
  fat16->length = length;

  Stream_free(root);
  Stream_free(fat);

  return base;
}
