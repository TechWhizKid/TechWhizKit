#include "sha256.h"

// The SHA-256 constants
static const uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

// The SHA-256 initial hash values
static const uint32_t H0[8] = {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

// A macro to perform a circular right shift of a 32-bit word
#define ROTR(x, n) (((x) >> (n)) | ((x) << (32 - (n))))

// A macro to perform a logical right shift of a 32-bit word
#define SHR(x, n) ((x) >> (n))

// The SHA-256 logical functions
#define CH(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define SIGMA0(x) (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define SIGMA1(x) (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define sigma0(x) (ROTR(x, 7) ^ ROTR(x, 18) ^ SHR(x, 3))
#define sigma1(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ SHR(x, 10))

// A helper function to convert a 32-bit word from big-endian to little-endian
static uint32_t swap_endian(uint32_t x)
{
    return ((x & 0xff000000) >> 24) |
           ((x & 0x00ff0000) >> 8) |
           ((x & 0x0000ff00) << 8) |
           ((x & 0x000000ff) << 24);
}

// A helper function to process a 512-bit block of data
static void sha256_transform(SHA256_CTX *ctx, const uint8_t data[])
{
    // Convert the data from big-endian to little-endian
    uint32_t W[64];
    for (int i = 0; i < 16; i++)
    {
        W[i] = swap_endian(((uint32_t *)data)[i]);
    }
    // Extend the first 16 words into the remaining 48 words
    for (int i = 16; i < 64; i++)
    {
        W[i] = sigma1(W[i - 2]) + W[i - 7] + sigma0(W[i - 15]) + W[i - 16];
    }
    // Initialize the working variables with the current state
    uint32_t a = ctx->state[0];
    uint32_t b = ctx->state[1];
    uint32_t c = ctx->state[2];
    uint32_t d = ctx->state[3];
    uint32_t e = ctx->state[4];
    uint32_t f = ctx->state[5];
    uint32_t g = ctx->state[6];
    uint32_t h = ctx->state[7];
    // Perform the main compression loop
    for (int i = 0; i < 64; i++)
    {
        uint32_t T1 = h + SIGMA1(e) + CH(e, f, g) + K[i] + W[i];
        uint32_t T2 = SIGMA0(a) + MAJ(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + T1;
        d = c;
        c = b;
        b = a;
        a = T1 + T2;
    }
    // Add the compressed chunk to the current state
    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
    ctx->state[5] += f;
    ctx->state[6] += g;
    ctx->state[7] += h;
}

// Initialize the SHA-256 context
void sha256_init(SHA256_CTX *ctx)
{
    // Copy the initial hash values into the state
    for (int i = 0; i < 8; i++)
    {
        ctx->state[i] = H0[i];
    }
    // Initialize the data buffer, the data length, and the bit length
    memset(ctx->data, 0, SHA256_BLOCK_SIZE);
    ctx->datalen = 0;
    ctx->bitlen = 0;
}

// Update the SHA-256 context with a chunk of data
void sha256_update(SHA256_CTX *ctx, const uint8_t data[], size_t len)
{
    // Loop through the data
    for (size_t i = 0; i < len; i++)
    {
        // Copy the data byte into the buffer
        ctx->data[ctx->datalen] = data[i];
        // Increment the data length
        ctx->datalen++;
        // If the buffer is full, process it
        if (ctx->datalen == SHA256_BLOCK_SIZE)
        {
            sha256_transform(ctx, ctx->data);
            // Update the bit length
            ctx->bitlen += SHA256_BLOCK_SIZE * 8;
            // Reset the data length
            ctx->datalen = 0;
        }
    }
}

// Finalize the SHA-256 context and produce the final digest
void sha256_final(SHA256_CTX *ctx, uint8_t hash[])
{
    // Pad the buffer with a single 1 bit, followed by zeros
    ctx->data[ctx->datalen] = 0x80;
    ctx->datalen++;
    // If the buffer does not have enough space for the length, process it
    if (ctx->datalen > SHA256_BLOCK_SIZE - 8)
    {
        // Fill the rest of the buffer with zeros
        while (ctx->datalen < SHA256_BLOCK_SIZE)
        {
            ctx->data[ctx->datalen] = 0x00;
            ctx->datalen++;
        }
        // Process the buffer
        sha256_transform(ctx, ctx->data);
        // Reset the data length
        ctx->datalen = 0;
    }
    // Fill the rest of the buffer with zeros, leaving 8 bytes for the length
    while (ctx->datalen < SHA256_BLOCK_SIZE - 8)
    {
        ctx->data[ctx->datalen] = 0x00;
        ctx->datalen++;
    }
    // Append the length in big-endian
    ctx->bitlen += ctx->datalen * 8;
    ctx->data[63] = ctx->bitlen;
    ctx->data[62] = ctx->bitlen >> 8;
    ctx->data[61] = ctx->bitlen >> 16;
    ctx->data[60] = ctx->bitlen >> 24;
    ctx->data[59] = ctx->bitlen >> 32;
    ctx->data[58] = ctx->bitlen >> 40;
    ctx->data[57] = ctx->bitlen >> 48;
    ctx->data[56] = ctx->bitlen >> 56;
    // Process the final block
    sha256_transform(ctx, ctx->data);
    // Convert the state from little-endian to big-endian and copy it to the hash
    for (int i = 0; i < 8; i++)
    {
        hash[i * 4] = (ctx->state[i] >> 24) & 0xff;
        hash[i * 4 + 1] = (ctx->state[i] >> 16) & 0xff;
        hash[i * 4 + 2] = (ctx->state[i] >> 8) & 0xff;
        hash[i * 4 + 3] = ctx->state[i] & 0xff;
    }
}
