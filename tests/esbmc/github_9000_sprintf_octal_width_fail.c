/* #9000: sprintf measures a field width and an octal or hex conversion. */
#include <stdio.h>
#include <stdlib.h>
unsigned char __VERIFIER_nondet_uchar();
int main()
{
  char *d = malloc(4);
  unsigned char c = __VERIFIER_nondet_uchar();
  int r = sprintf(d, "\\%03o", c);
  free(d);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9000_sprintf_octal_width_fail
// RUN: --force-malloc-success --no-incremental-bmc
// EXPECT: ^  dereference failure: array bounds violated: heap object$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  dereference failure: array bounds violated: heap object$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
