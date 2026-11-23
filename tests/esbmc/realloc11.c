#include <stdlib.h>

int main() {
  int *p = malloc(sizeof(int));
  if (!p) return 0;
  *p = 7;
  int *q = realloc(p, 1000 * sizeof(int));
  if (q == NULL) {
    // allocation failed, p is still valid and unchanged
    __VERIFIER_assert(*p == 7); // Old pointer must remain valid after realloc fail
    free(p);
  } else {
    // success
    __VERIFIER_assert(q[0] == 7); // Reallocated pointer must preserve old data
    free(q);
  }
  return 0;
}

// Imported from ESBMC regression/esbmc/realloc11
// Needs: --compact-trace
// RUN: --force-malloc-success --compact-trace
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
