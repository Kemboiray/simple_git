#include "main.h"

int calculate_file_sha1(FILE *stream, unsigned char *digest,
                        unsigned int *digest_len) {
  EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
  if (!mdctx) {
    perror("EVP_MD_CTX_new");
    return 1;
  }

  if (!EVP_DigestInit_ex(mdctx, EVP_sha1(), NULL)) {
    perror("EVP_DigestInit_ex");
    EVP_MD_CTX_free(mdctx);
    return 1;
  }

  // Read file in chunks
  unsigned char buffer[4096];
  size_t bytes;

  while ((bytes = fread(buffer, 1, sizeof(buffer), stream)) != 0) {
    if (!EVP_DigestUpdate(mdctx, buffer, bytes)) {
      perror("EVP_DigestUpdate");
      EVP_MD_CTX_free(mdctx);
      return 1;
    }
  }

  if (!EVP_DigestFinal_ex(mdctx, digest, digest_len)) {
    perror("EVP_DigestFinal_ex");
    EVP_MD_CTX_free(mdctx);
    return 1;
  }

  EVP_MD_CTX_free(mdctx);
  return 0;
}

char *hash_file(FILE *stream, char *buffer, size_t buffer_size) {
  unsigned char digest[EVP_MAX_MD_SIZE] = {0};
  unsigned int digest_len;

  if (!buffer || buffer_size < EVP_MAX_MD_SIZE * 2 + 1) {
    fputs("Insufficient hash buffer size", stderr);
    return NULL;
  }

  if (calculate_file_sha1(stream, digest, &digest_len) != 0) {
    return NULL;
  }

  for (unsigned int i = 0; i < digest_len; i++) {
    sprintf(buffer + (i * 2), "%02x", digest[i]);
  }

  return buffer;
}
