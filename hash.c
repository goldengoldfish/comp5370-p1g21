
#include <stdio.h>
#include <openssl/evp.h>
#include "hash.h"

int hash_file_sha256(const char *path, char *output, size_t output_size) {
    if (path == NULL || output == NULL) {
        return 1;
    }

    if (output_size < 65) {
        return 2;
    }

    output[0] = '\0';

    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        return 3;
    }

    EVP_MD_CTX *context = EVP_MD_CTX_new();
    if (context == NULL) {
        fclose(file);
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

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (EVP_DigestUpdate(context, buffer, bytes_read) != 1) {
            result = 4;
            goto cleanup;
        }
    }

    if (ferror(file)) {
        result = 5;
        goto cleanup;
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
    fclose(file);

    if (result != 0) {
        output[0] = '\0';
    }

    return result;
}
