#include "drive.h"
#include "file.h"
#include "hash.h"
#include "partition.h"
#include "stream.h"
#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

static bool checkDir(const char* path) {
  DIR *dir = opendir(path);
  if (!dir) {
    perror(path);
    return false;
  }
  closedir(dir);
  return true;
}

static void listNode(FileNode *node, size_t indent) {
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
        listNode(FileNode_getChild(node, i), indent + 1);
      }
    default:
      break;
  }
  free(hash);
}

static void listPartitions(Drive *drive) {
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
  }
}

static int carvePartitions(Drive *drive, const char* outputDir) {
  if (!checkDir(outputDir)) {
    return 1;
  }

  size_t pathMaxLen = strlen(outputDir) + 32;
  char *path = malloc(pathMaxLen);

  unsigned char temp[512];
  Stream *image = Drive_getImage(drive);

  for (size_t i = 0; i < Drive_getPartitionCount(drive); i++) {
    Partition *p = Drive_getPartition(drive, i);
    snprintf(path, pathMaxLen, "%s/%zd", outputDir, i);

    FILE *out = fopen(path, "wb");
    if (!out) {
      perror("Carving partition");
      return 1;
    }

    Stream *in = Stream_new_fromStream(image, Partition_getOffset(p), Partition_getLength(p));
    while (!Stream_isEof(in)) {
      size_t numRead = Stream_read(in, temp, 0, sizeof(temp));
      fwrite(temp, 1, numRead, out);
      assert(!ferror(out) && "Failed to write carved partition");
    }

    fclose(out);
  }

  printf("Carved %zd partitions\n", Drive_getPartitionCount(drive));
  return 0;
}

static int carveNode(FileNode *node, const char *outputDir) {
  if (!checkDir(outputDir)) {
    return 1;
  }

  unsigned char temp[512];

  switch (FileNode_getKind(node)) {
    case FNK_FILE: {
      size_t nameLen = strlen(outputDir) + 1 + strlen(FileNode_getName(node)) + 1;
      char *name = malloc(nameLen);
      snprintf(name, nameLen, "%s/%s", outputDir, FileNode_getName(node));

      FILE *out = fopen(name, "wb");
      if (!out) {
        perror(name);
        return 1;
      }

      Stream *in = FileNode_open(node);
      Stream_seek(in, 0);
      while (!Stream_isEof(in)) {
        size_t numRead = Stream_read(in, temp, 0, sizeof(temp));
        fwrite(temp, 1, numRead, out);
        assert(!ferror(out) && "Failed to write carved file");
      }

      fclose(out);
      break;
    }
    case FNK_ROOT:
      for (size_t i = 0; i < FileNode_getChildCount(node); i++) {
        int r = carveNode(FileNode_getChild(node, i), outputDir);
        if (r) {
          return r;
        }
      }
      break;
    case FNK_DIRECTORY: {
      size_t folderLen = strlen(outputDir) + 1 + strlen(FileNode_getName(node)) + 1;
      char *folder = malloc(folderLen);
      snprintf(folder, folderLen, "%s/%s", outputDir, FileNode_getName(node));
      if (mkdir(folder, 0700)) {
        perror(folder);
        return 1;
      }
      for (size_t i = 0; i < FileNode_getChildCount(node); i++) {
        int r = carveNode(FileNode_getChild(node, i), folder);
        if (r) {
          return r;
        }
      }
      break;
    }
    default:
      break;
  }

  return 0;
}

int main(int argc, char **argv) {
  assert(argc >= 3 && "usage: da <image> <output>");
  Drive *drive = Drive_new(argv[1]);

  int result = 0;
  puts("=== Partitions ===");
  listPartitions(drive);
  carvePartitions(drive, argv[2]);


  for (size_t i = 0; i < Drive_getPartitionCount(drive); i++) {
    Partition *p = Drive_getPartition(drive, i);
    FileNode *n = Partition_getRoot(p);
    printf("== Files on %zd ===\n", i);
    listNode(n, 0);
  }

  Drive_free(drive);
  return result;
}
