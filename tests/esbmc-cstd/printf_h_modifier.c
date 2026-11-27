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
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check -m32
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict FAILED
