// llvm.bitreverse reverses the bits of scalar integers.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdint.h>

extern unsigned __VERIFIER_nondet_uint(void);
extern unsigned long long __VERIFIER_nondet_ulonglong(void);
extern void __VERIFIER_assert(int);

int main(void) {
  uint32_t x = __VERIFIER_nondet_uint();
  if (x == 0u)
    __VERIFIER_assert(__builtin_bitreverse32(x) == 0u);
  if (x == ~0u)
    __VERIFIER_assert(__builtin_bitreverse32(x) == ~0u);
  if (x == 0x12345678u)
    __VERIFIER_assert(__builtin_bitreverse32(x) == 0x1e6a2c48u);

  uint32_t rx = __builtin_bitreverse32(x);
  __VERIFIER_assert(__builtin_bitreverse32(rx) == x);
  __VERIFIER_assert(((rx >> 31) & 1u) == (x & 1u));
  __VERIFIER_assert((rx & 1u) == ((x >> 31) & 1u));
  __VERIFIER_assert(__builtin_bitreverse32(~x) == ~rx);
  __VERIFIER_assert(__builtin_popcount(rx) == __builtin_popcount(x));

  uint64_t y = __VERIFIER_nondet_ulonglong();
  if (y == 1ULL)
    __VERIFIER_assert(__builtin_bitreverse64(y) == (1ULL << 63));
  if (y == 0x0123456789abcdefULL)
    __VERIFIER_assert(__builtin_bitreverse64(y) == 0xf7b3d591e6a2c480ULL);
  uint64_t ry = __builtin_bitreverse64(y);
  __VERIFIER_assert(__builtin_bitreverse64(ry) == y);
  __VERIFIER_assert(((ry >> 63) & 1ULL) == (y & 1ULL));
  __VERIFIER_assert(__builtin_bitreverse64(~y) == ~ry);
  return 0;
}
