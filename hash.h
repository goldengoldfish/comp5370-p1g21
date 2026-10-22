
#ifndef HASH_H
#define HASH_H

#include <stddef.h>
#include "stream.h"

/*
 * Calculates the SHA-256 hash of a file.
 *
 * stream: the stream to hash
 * output: Buffer that receives the 64-character hexadecimal hash.
 *
 * Returns 0 on success and a nonzero value on failure.
 */
int hash_file_sha256(Stream *stream, char *output);

#endif
