// #10301: a discarded realloc result crashed GOTO conversion.
#include <stdlib.h>
int main()
{
  int *q = (int *)1;
  realloc(q, sizeof(int));
}
// Imported from ESBMC regression/esbmc/github_10301_discard_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: FAILED.*invalid-pointer-freed.*line 6.*dereference failure: invalid pointer freed$
