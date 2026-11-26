// #8006
#include <stdlib.h>

int main()
{
  int *p = malloc(sizeof(int));
  if (!p)
    return 0;
  *p = 1;
  int a = *p + 1;
  free(p);
  int b = *p + 1; // use after free
  return a + b;
}
// Imported from ESBMC regression/esbmc/github_8006
// RUN:
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict SUCCESSFUL
