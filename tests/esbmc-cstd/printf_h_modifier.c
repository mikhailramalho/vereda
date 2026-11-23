#include <assert.h>
#include <stdio.h>

int main()
{
  /* %hd truncates to short: 70000 & 0xFFFF = 4464 → 4 chars */
  int x = printf("%hd", 70000);
#ifndef _WIN32
  assert(x == 4);
#endif
}
// Imported from ESBMC regression/cstd/printf_h_modifier
// Needs: REQUIRES arch32
// RUN: --overflow-check --32
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
