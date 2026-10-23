#include "drive.h"
#include "file.h"
#include "hash.h"
#include "partition.h"
#include "stream.h"
#include <assert.h>
#include <stdio.h>

static void dumpNode(FileNode *node, size_t indent) {
  for (size_t i = 0; i < indent; i++) {
    putc('\t', stdout);
  }
  char *hash = malloc(65);
  switch (FileNode_getKind(node)) {
    case FNK_FILE: {
      // No need to free stream since it's owned by the FileNode
      Stream *s = FileNode_open(node);
      Stream_seek(s, 0);
      hash_file_sha256(s, hash);
      printf("%s [%s]\n", FileNode_getName(node), hash);
      break;
    }
    case FNK_ROOT:
    case FNK_DIRECTORY:
      printf("%s/\n", FileNode_getName(node));
      for (size_t i = 0; i < FileNode_getChildCount(node); i++) {
        dumpNode(FileNode_getChild(node, i), indent + 1);
      }
    default:
      break;
  }
  free(hash);
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
