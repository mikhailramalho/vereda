// #9895: a double free is a bad free, not a dereference.
#include <stdlib.h>

int main()
{
  char *p = malloc(4);
  free(p);
  free(p);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9895_free_twice
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: dereference failure: invalidated dynamic object freed
// EXPECT: \A(?![\s\S]*invalidated dynamic object$)[\s\S]*\Z
