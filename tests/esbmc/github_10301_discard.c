// #10301: a discarded realloc result crashed GOTO conversion.
#include <stdlib.h>
int main()
{
  int *q = malloc(sizeof(int));
  realloc(q, 2 * sizeof(int));
}
// Imported from ESBMC regression/esbmc/github_10301_discard
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
