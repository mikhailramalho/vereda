// Reproducer for non-canonical x86_fp80 pseudo-denormal with symbolic fraction:
// exp=0, intBit=1 represents >= LDBL_MIN, so assert(u.x < LDBL_MIN) must fail.
// RUN: -m64
// RUN: -m64 --fp2bv
// EXPECT: ^VERIFICATION FAILED$
#include <float.h>
#include <stdint.h>

extern void __VERIFIER_assert(int);
extern uint64_t __VERIFIER_nondet_uint64(void);

union ld_bits {
  long double x;
  struct {
    uint64_t sig;
    uint16_t exp;
  } b;
};

int main(void) {
  union ld_bits u;
  u.b.exp = 0;
  u.b.sig = 0x8000000000000000ULL | (__VERIFIER_nondet_uint64() & 0x7fffffffffffffffULL);
  __VERIFIER_assert(u.x < LDBL_MIN);
  return 0;
}
