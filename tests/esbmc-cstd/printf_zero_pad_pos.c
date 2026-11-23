#include <assert.h>
#include <stdio.h>

int main()
{
  /* "%05d" with 3 → "00003" (5 chars) */
  int x = printf("%05d", 3);
#ifndef _WIN32
  assert(x == 5);
#endif
}
// Imported from ESBMC regression/cstd/printf_zero_pad_pos
// Needs: REQUIRES arch32
// RUN: --overflow-check --32
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
