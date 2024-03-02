#ifndef SHA256_H
#define SHA256_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define SHA256_BLOCK_SIZE 64  // The SHA-256 block size in bytes
#define SHA256_DIGEST_SIZE 32 // The SHA-256 digest size in bytes

// The SHA-256 context structure
typedef struct
{
    uint8_t data[SHA256_BLOCK_SIZE]; // The data buffer
    uint32_t datalen;                // The length of the data buffer
    uint64_t bitlen;                 // The total length of the input data in bits
    uint32_t state[8];               // The state vector
} SHA256_CTX;

void sha256_init(SHA256_CTX *ctx);                                     // Initialize the SHA-256 context
void sha256_update(SHA256_CTX *ctx, const uint8_t data[], size_t len); // Update the SHA-256 context with a chunk of data
void sha256_final(SHA256_CTX *ctx, uint8_t hash[]);                    // Finalize the SHA-256 context and produce the final digest
void sha256_toString(const uint8_t hash[], char output[]);             // Convert a byte array to a hexadecimal string
void sha256_hash(const char *data, char output[]);                     // Hash a string and return the hexadecimal digest

#endif
