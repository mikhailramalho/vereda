#include <stdlib.h>

int main(int argc, char **argv) {
  int *a = (int *)malloc(sizeof(int));

  int *temp = realloc(a, 2 * sizeof(int));
  a = temp;

  free(a);
  return 0;
}
// Imported from ESBMC regression/esbmc/realloc7
// Needs: --force-realloc-success --memory-leak-check
// RUN: --force-realloc-success --force-malloc-success --memory-leak-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
