#include <stdlib.h>
int main() {
  int *a = malloc(sizeof(int));
  if (!a) return 0;
  a = realloc(a, 2 * sizeof(int));
  free(a);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8554_old_leak_fail
// Needs: --memory-leak-check --no-reachable-memory-leak
// RUN: --memory-leak-check --no-reachable-memory-leak --incremental-bmc
// MODE: FUTURE
// EXPECT: forgotten memory: dynamic_\d+_value$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
