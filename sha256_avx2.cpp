#include "sha256_avx2.h"
#include <string.h>
#include <stdint.h>

namespace _sha256 {

    // Initialize SHA-256 state with initial hash values
    void Initialize(uint32_t* s) {
        const uint32_t init[8] = {
            0x6a09e667,
            0xbb67ae85,
            0x3c6ef372,
            0xa54ff53a,
            0x510e527f,
            0x9b05688c,
            0x1f83d9ab,
            0x5be0cd19
        };

        for (int i = 0; i < 8; ++i) {
            s[i] = init[i];
        }
    }

    // SHA-256 macros without AVX2 intrinsics
    #define Maj(x, y, z) ((x & y) ^ (z & (x | y)))
    #define Ch(x, y, z) ((x & y) ^ (~x & z))
    #define ROR(x, n) ((x >> n) | (x << (32 - n)))
    #define SHR(x, n) (x >> n)

    #define S0(x) (ROR(x, 2) ^ ROR(x, 13) ^ ROR(x, 22))
    #define S1(x) (ROR(x, 6) ^ ROR(x, 11) ^ ROR(x, 25))
    #define s0(x) (ROR(x, 7) ^ ROR(x, 18) ^ (x >> 3))
    #define s1(x) (ROR(x, 17) ^ ROR(x, 19) ^ (x >> 10))

    #define Round(a, b, c, d, e, f, g, h, Kt, Wt) \
        T1 = h + S1(e) + Ch(e, f, g) + Kt + Wt; \
        T2 = S0(a) + Maj(a, b, c); \
        h = g; \
        g = f; \
        f = e; \
        e = d + T1; \
        d = c; \
        c = b; \
        b = a; \
        a = T1 + T2;

    void Transform(uint32_t* state, const uint8_t* data[8]) {
        uint32_t a, b, c, d, e, f, g, h;
        uint32_t W[64];
        uint32_t T1, T2;

        // Load state into local variables
        a = state[0];
        b = state[1];
        c = state[2];
        d = state[3];
        e = state[4];
        f = state[5];
        g = state[6];
        h = state[7];

        // Prepare message schedule W[0..15]
        for (int t = 0; t < 16; ++t) {
            const uint8_t* ptr = data[t % 8]; // Use the appropriate data pointer
            W[t] = ((uint32_t)ptr[0] << 24) | ((uint32_t)ptr[1] << 16) | ((uint32_t)ptr[2] << 8) | ((uint32_t)ptr[3]);
        }

        for (int t = 16; t < 64; ++t) {
            W[t] = s1(W[t - 2]) + W[t - 7] + s0(W[t - 15]) + W[t - 16];
        }

        // Constants
        static const uint32_t K[64] = {
            0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
            0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
            0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
            0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
           