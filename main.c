#include "drive.h"
#include "file.h"
#include "partition.h"
#include <assert.h>
#include <stdio.h>
#include "args.h"

static void dumpNode(FileNode *node, size_t indent) {
  for (size_t i = 0; i < indent; i++) {
    putc('\t', stdout);
  }
  switch (FileNode_getKind(node)) {
  case FNK_FILE:
    printf("%s\n", FileNode_getName(node));
    break;
  case FNK_ROOT:
  case FNK_DIRECTORY:
    printf("%s/\n", FileNode_getName(node));
    for (size_t i = 0; i < FileNode_getChildCount(node); i++) {
      dumpNode(FileNode_getChild(node, i), indent + 1);
    }
  default:
    break;
  }
}

int main(int argc, char **argv) {
  assert(argc >= 2 && "usage: da <image>");
  Drive *drive = Drive_new(argv[1]);

  for (size_t i = 0; i < Drive_getPartitionCount(drive); i++) {
    Partition *p = Drive_getPartition(drive, i);
    printf("Partition %zd:\n", i);
    printf("\tPartition Type: ");
    switch (Partition_getKind(p)) {
      case PK_FAT16:
        puts("FAT16");
        break;
      default:
        puts("Unknown");
        break;
    }
    printf("\tStart Offset: %zd (%zd secotrs)\n", Partition_getOffset(p), Partition_getOffset(p) / 512);
    printf("\tLength: %zd (%zd sectors)\n", Partition_getLength(p), Partition_getLength(p) / 512);
    printf("\tFiles:\n");
    dumpNode(Partition_getRoot(p), 1);
  }

  Drive_free(drive);
  return 0;
}
