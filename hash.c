#include <stdio.h>
#include <openssl/evp.h>
#include "hash.h"
#include "stream.h"

int hash_file_sha256(Stream *stream, char *output) {
    if (output == NULL) {
        return 1;
    }

    output[64] = '\0';

    EVP_MD_CTX *context = EVP_MD_CTX_new();
    if (context == NULL) {
        return 4;
    }

    unsigned char buffer[8192];
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_length = 0;
    int result = 0;

    if (EVP_DigestInit_ex(context, EVP_sha256(), NULL) != 1) {
        result = 4;
        goto cleanup;
    }

    size_t bytes_read;
    while ((bytes_read = Stream_read(stream, buffer, 0, sizeof(buffer)) > 0)) {
        if (EVP_DigestUpdate(context, buffer, bytes_read) != 1) {
            result = 4;
            goto cleanup;
        }
    }

    if (EVP_DigestFinal_ex(context, digest, &digest_length) != 1) {
        result = 4;
        goto cleanup;
    }

    for (unsigned int i = 0; i < digest_length; i++) {
        snprintf(&output[i * 2], 3, "%02x", digest[i]);
    }

cleanup:
    EVP_MD_CTX_free(context);

    if (result != 0) {
        output[0] = '\0';
    }

    return result;
}
