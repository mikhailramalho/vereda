#include <stdlib.h>
int main() {
  int *a = malloc(sizeof(int));
  if (!a) return 0;
  size_t n = __VERIFIER_nondet_ulong();
  int *t = realloc(a, n);
  if (t) a = t;
  free(a);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8554_over_cap
// Needs: --memory-leak-check --no-reachable-memory-leak
// RUN: --memory-leak-check --no-reachable-memory-leak --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
