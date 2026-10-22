
#ifndef HASH_H
#define HASH_H

#include <stddef.h>

/*
 * Calculates the SHA-256 hash of a file.
 *
 * path: Path to the file to hash.
 * output: Buffer that receives the 64-character hexadecimal hash.
 * output_size: Size of output; must be at least 65 bytes.
 *
 * Returns 0 on success and a nonzero value on failure.
 */
int hash_file_sha256(const char *path, char *output, size_t output_size);

#endif
