#include <assert.h>
#include <stdint.h>
#include <stdio.h>

int main()
{
  /* %jd uses intmax_t (always 64-bit); 3e9 overflows 32-bit long.
     Correct: "3000000000" = 10 chars; wrong (long on --32): 11 chars. */
  int x = printf("%jd", (intmax_t)3000000000LL);
#ifndef _WIN32
  assert(x == 10);
#endif
}
// Imported from ESBMC regression/cstd/printf_j_modifier
// RUN: --overflow-check -m32
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict FAILED
