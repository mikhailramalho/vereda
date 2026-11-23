/* #9354: fputs_unlocked reads a freed string. */
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
  char *p = malloc(4);
  if (!p)
    return 0;
  p[0] = 'a';
  p[1] = 0;
  free(p);
  fputs_unlocked(p, stdout);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_9354_fputs_uaf_fail
// RUN: -m64 --unwind 8
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: FAILED +\[strlen\.invalidated-dynamic-object\.[0-9]+\].*dereference failure: invalidated dynamic object
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
