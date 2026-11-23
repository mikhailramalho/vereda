#include <stdlib.h>

int main()
{
  int *a = malloc(2);
  exit(0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_2335_6
// Needs: --memory-leak-check
// RUN: --memory-leak-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
