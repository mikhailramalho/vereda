#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

struct D
{
  int dummy;
  char fmt[16];
};

int main()
{
  unsigned n = __VERIFIER_nondet_uint() % 1024;
  n++;
  unsigned i = __VERIFIER_nondet_uint() % n;
  struct D data[1];
#ifndef _WIN32
  char* ffmt = "%02X";
  int r = printf(ffmt, data[0].fmt);
  assert(r < 2);
#else
  assert(0);
#endif
}
// Imported from ESBMC regression/cstd/printf_8
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
