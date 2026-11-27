#include <assert.h>
#include <stdio.h>

int main()
{
  /* "%05d" with -3 → "-0003" (5 chars, not "000-3") */
  int x = printf("%05d", -3);
#ifndef _WIN32
  assert(x == 5);
#endif
}
// Imported from ESBMC regression/cstd/printf_zero_pad_neg
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check -m32 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict FAILED
