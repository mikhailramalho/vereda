/* #9000: sprintf measures a field width and an octal or hex conversion. */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
unsigned short __VERIFIER_nondet_ushort();
int main()
{
  char *d = malloc(5);
  unsigned short c = __VERIFIER_nondet_ushort();
  int r = sprintf(d, "%-2X", c);
  assert(r >= 2 && r <= 4);
  assert(r == 4 || c < 0x1000);
  free(d);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9000_sprintf_hex_width
// RUN: --force-malloc-success
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
