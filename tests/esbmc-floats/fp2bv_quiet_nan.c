#include <assert.h>
#include <string.h>

unsigned __VERIFIER_nondet_uint(void);
unsigned long long __VERIFIER_nondet_ulonglong(void);

int main(void)
{
  unsigned zb = __VERIFIER_nondet_uint();
  __VERIFIER_assume(zb == 0);
  float z;
  memcpy(&z, &zb, sizeof z);
  float f = z / z;
  unsigned fbits;
  memcpy(&fbits, &f, sizeof fbits);
  assert(fbits == 0x7fc00000u);

  unsigned long long zdb = __VERIFIER_nondet_ulonglong();
  __VERIFIER_assume(zdb == 0);
  double zd;
  memcpy(&zd, &zdb, sizeof zd);
  double d = zd / zd;
  unsigned long long dbits;
  memcpy(&dbits, &d, sizeof dbits);
  assert(dbits == 0x7ff8000000000000ull);
  return 0;
}
// Imported from ESBMC regression/floats/fp2bv_quiet_nan
// RUN: --fp2bv --solver bitwuzla --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
