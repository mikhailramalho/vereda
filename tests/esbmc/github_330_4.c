#include <stdlib.h>

int main(argc, argv) char **argv;
{
  int *foo = __builtin_alloca(sizeof(int));
  return 0;
}

// Imported from ESBMC regression/esbmc/github_330_4
// Needs: --memory-leak-check
// RUN: --memory-leak-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
