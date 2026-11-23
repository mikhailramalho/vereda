#include <stdlib.h>
#include <string.h>

int main(void)
{
  char *p = malloc(4);
  if (!p)
    return 0;
  memcpy(p, "PING", 4);
  int r = memcmp(p, "HELLO-V1", 8);
  free(p);
  return r != 0;
}
// Imported from ESBMC regression/cstd/github_9449_fail
// RUN: --unwind 20
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: dereference failure: array bounds violated: heap object$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
