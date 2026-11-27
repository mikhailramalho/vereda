#include <stdlib.h>

int main()
{
  int *p = realloc(NULL, sizeof(int));
  *p = 42;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_1462
// RUN: --force-malloc-success
// EXPECT: ^VERIFICATION SUCCESSFUL$
