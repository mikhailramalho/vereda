#include <stdlib.h>

int **g = NULL;

void free_g1()
{
  free(g);
  g = NULL;
}

void free_g2()
{
  if(g != NULL)
    free(*g);
}

void f()
{
  *g = (int *)malloc(sizeof(int));
  atexit(free_g1);
  exit(1);
}

int main()
{
  g = (int **)malloc(sizeof(int *));
  atexit(free_g2);
  f();
  return 0;
}
// Imported from ESBMC regression/esbmc/github_542_fail
// Needs: --memory-leak-check
// RUN: --force-malloc-success --memory-leak-check --no-unwinding-assertions --unwind 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
