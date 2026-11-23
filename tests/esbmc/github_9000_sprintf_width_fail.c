/* #9000: sprintf measures a field width and an octal or hex conversion. */
#include <stdio.h>
#include <stdlib.h>
unsigned char __VERIFIER_nondet_uchar();
int main()
{
  char *d = malloc(5);
  unsigned char c = __VERIFIER_nondet_uchar();
  sprintf(d, "%5d", c % 10);
  free(d);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9000_sprintf_width_fail
// RUN: --force-malloc-success
// EXPECT: ^  dereference failure: array bounds violated: heap object$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  dereference failure: array bounds violated: heap object$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
