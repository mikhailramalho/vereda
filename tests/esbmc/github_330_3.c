#include <stdlib.h>

int main(argc, argv) char **argv;
{
  int *foo = __builtin_alloca(sizeof(int));
  free(foo);
  return 0;
}

// Imported from ESBMC regression/esbmc/github_330_3
// Needs: --memory-leak-check
// RUN: --memory-leak-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
