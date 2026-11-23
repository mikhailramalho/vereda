#include <stdlib.h>
int main() {
  int *a = malloc(sizeof(int));
  if (!a) return 0;
  int *t = realloc(a, 2 * sizeof(int));
  if (!t) { free(a); return 0; }
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8554_new_leak_fail
// Needs: --memory-leak-check --no-reachable-memory-leak
// RUN: --memory-leak-check --no-reachable-memory-leak --incremental-bmc
// MODE: FUTURE
// EXPECT: forgotten memory: realloc_\d+_array$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
