#include <stdlib.h>
int main() {
  int *a = malloc(sizeof(int));
  if (!a) return 0;
  int *t = realloc(a, 2 * sizeof(int));
  if (t) a = t;
  free(a);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8554
// Needs: --memory-leak-check --no-reachable-memory-leak
// RUN: --memory-leak-check --no-reachable-memory-leak --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
