// Non-canonical x86_fp80 bit patterns constructed via union/memory with both
// symbolic and constant bits: pseudo-denormals normalize to exp == 1 (>= LDBL_MIN),
// while unnormals, pseudo-infinities, and pseudo-NaNs (exp != 0, bit 63 == 0)
// behave as NaN / unordered, matching x87 hardware.
// RUN: -m64
// RUN: -m64 --fp2bv
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <float.h>
#include <math.h>
#include <stdint.h>

extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
extern uint64_t __VERIFIER_nondet_uint64(void);
extern uint16_t __VERIFIER_nondet_uint16(void);

union ld_bits {
  long double x;
  struct {
    uint64_t sig;
    uint16_t exp;
  } b;
};

int main(void) {
  // Constant pseudo-denormal: exp=0, intBit=1, frac=0 equals LDBL_MIN.
  union ld_bits c_pd;
  c_pd.b.exp = 0x0000;
  c_pd.b.sig = 0x8000000000000000ULL;
  __VERIFIER_assert(c_pd.x == LDBL_MIN);
  __VERIFIER_assert(!(c_pd.x < LDBL_MIN));
  __VERIFIER_assert(isfinite(c_pd.x) && !isnan(c_pd.x));

  // Symbolic pseudo-denormal: exp=0, intBit=1, symbolic frac >= LDBL_MIN.
  uint64_t frac = __VERIFIER_nondet_uint64() & 0x7fffffffffffffffULL;
  union ld_bits s_pd;
  s_pd.b.exp = 0x0000;
  s_pd.b.sig = 0x8000000000000000ULL | frac;
  __VERIFIER_assert(s_pd.x >= LDBL_MIN);
  __VERIFIER_assert(!(s_pd.x < LDBL_MIN));
  __VERIFIER_assert(isfinite(s_pd.x) && !isnan(s_pd.x));
  if (frac == 0)
    __VERIFIER_assert(s_pd.x == LDBL_MIN);
  else
    __VERIFIER_assert(s_pd.x > LDBL_MIN);

  // Symbolic unnormal / pseudo-infinity / pseudo-NaN: exp != 0, intBit == 0 -> NaN.
  uint16_t e = __VERIFIER_nondet_uint16() & 0x7fff;
  __VERIFIER_assume(e != 0);
  union ld_bits s_un;
  s_un.b.exp = e;
  s_un.b.sig = frac; // bit 63 is 0
  __VERIFIER_assert(isnan(s_un.x));
  __VERIFIER_assert(!isfinite(s_un.x) && !isinf(s_un.x));
  __VERIFIER_assert(!(s_un.x == s_un.x));
  __VERIFIER_assert(!(s_un.x < 1.0L) && !(s_un.x >= 1.0L));

  // Constant unnormal and pseudo-infinity: both NaN.
  union ld_bits c_un;
  c_un.b.exp = 0x3fff;
  c_un.b.sig = 0;
  __VERIFIER_assert(isnan(c_un.x));
  union ld_bits c_pinf;
  c_pinf.b.exp = 0x7fff;
  c_pinf.b.sig = 0;
  __VERIFIER_assert(isnan(c_pinf.x) && !isinf(c_pinf.x));

  return 0;
}
