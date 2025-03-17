#include "ripemd160_avx2.h"
#include <cstring>
#include <cstdint>

namespace ripemd160 {

#ifdef WIN64
static const uint32_t _init[] = {
#else
static const uint32_t _init[] __attribute__((aligned(32))) = {
#endif
    // 8 copies of A
    0x67452301ul, 0x67452301ul, 0x67452301ul, 0x67452301ul,
    0x67452301ul, 0x67452301ul, 0x67452301ul, 0x67452301ul,

    // 8 copies of B
    0xEFCDAB89ul, 0xEFCDAB89ul, 0xEFCDAB89ul, 0xEFCDAB89ul,
    0xEFCDAB89ul, 0xEFCDAB89ul, 0xEFCDAB89ul, 0xEFCDAB89ul,

    // 8 copies of C
    0x98BADCFEul, 0x98BADCFEul, 0x98BADCFEul, 0x98BADCFEul,
    0x98BADCFEul, 0x98BADCFEul, 0x98BADCFEul, 0x98BADCFEul,

    // 8 copies of D
    0x10325476ul, 0x10325476ul, 0x10325476ul, 0x10325476ul,
    0x10325476ul, 0x10325476ul, 0x10325476ul, 0x10325476ul,

    // 8 copies of E
    0xC3D2E1F0ul, 0xC3D2E1F0ul, 0xC3D2E1F0ul, 0xC3D2E1F0ul,
    0xC3D2E1F0ul, 0xC3D2E1F0ul, 0xC3D2E1F0ul, 0xC3D2E1F0ul
};

// RIPEMD-160 functions
#define f1(x, y, z) ((x) ^ (y) ^ (z))
#define f2(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define f3(x, y, z) (((x) | ~(y)) ^ (z))
#define f4(x, y, z) (((x) & (z)) | ((y) & ~(z)))
#define f5(x, y, z) ((x) ^ ((y) | ~(z)))

// Adding helpers
#define add3(x0, x1, x2) ((x0) + (x1) + (x2))
#define add4(x0, x1, x2, x3) ((x0) + (x1) + (x2) + (x3))

// Round function
#define Round(a, b, c, d, e, f, x, k, r)   \
    u = add4(a, f, x, k); \
    a = add3(u, e); \
    c = ((c) << 10) | ((c) >> (32 - 10));

// Macros for each round
#define R11(a, b, c, d, e, x, r) Round(a, b, c, d, e, f1(b, c, d), x, r)
#define R21(a, b, c, d, e, x, r) Round(a, b, c, d, e, f2(b, c, d), x, 0x5A827999ul, r)
#define R31(a, b, c, d, e, x, r) Round(a, b, c, d, e, f3(b, c, d), x, 0x6ED9EBA1ul, r)
#define R41(a, b, c, d, e, x, r) Round(a, b, c, d, e, f4(b, c, d), x, 0x8F1BBCDCul,