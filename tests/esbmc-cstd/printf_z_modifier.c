#include <assert.h>
#include <stddef.h>
#include <stdio.h>

int main()
{
  /* %zu uses size_t (config-derived width) */
  int x = printf("%zu", (size_t)12345);
#ifndef _WIN32
  assert(x == 5);
#endif
}
// Imported from ESBMC regression/cstd/printf_z_modifier
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check -m32 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict FAILED
