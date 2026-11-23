#include <stdlib.h>

extern int __VERIFIER_nondet_int(void);

int main() {
  int size = __VERIFIER_nondet_int();
  if (size <= 0 || size > 5) return 0;

  int *p = malloc(size * sizeof(int));
  if (!p) return 0;

  for (int i = 0; i < size; i++) p[i] = i;

  int newsize = __VERIFIER_nondet_int();
  if (newsize <= size || newsize > 100) return 0; // grow

  int *q = realloc(p, newsize * sizeof(int));
  if (q == NULL) {
    // realloc failed: old pointer must remain valid
    for (int i = 0; i < size; i++) {
      __VERIFIER_assert(p[i] == i); // Old pointer must remain valid after realloc fail
    }
    free(p);
  } else {
    // realloc succeeded: new pointer must preserve old data
    for (int i = 0; i < size; i++) {
      __VERIFIER_assert(q[i] == i); // Reallocated pointer must preserve old data
    }
    free(q);
  }

  return 0;
}

// Imported from ESBMC regression/esbmc/realloc16
// RUN: --unwind 6
// MODE: THOROUGH
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
