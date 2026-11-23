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
// Needs: REQUIRES arch32
// RUN: --overflow-check --32
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
