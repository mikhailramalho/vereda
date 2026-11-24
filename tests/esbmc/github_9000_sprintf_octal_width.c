/* #9000: sprintf measures a field width and an octal or hex conversion. */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
unsigned char __VERIFIER_nondet_uchar();
int main()
{
  char *d = malloc(5);
  unsigned char c = __VERIFIER_nondet_uchar();
  int r = sprintf(d, "\\%03o", c);
  assert(r == 4 && d[4] == 0 && d[3] != 0);
  free(d);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9000_sprintf_octal_width
// RUN: --force-malloc-success
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: FUTURE test passed unexpectedly; promote it to CORE
// MODE: CORE
