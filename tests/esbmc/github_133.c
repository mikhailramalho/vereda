/* Out-of-bounds accesses: (*e).b[3] lies past the three ints the calloc paid
 * for, and a.b[0] reads storage the global `a` does not have, since a flexible
 * array member adds none (C17 6.7.2.1p18, #5393). */
#include<stdlib.h>
#include<assert.h>

struct A {
  char alloc;
  int b[];
} a;

int main() {
  struct A *e = calloc(1, sizeof(struct A) + 3*sizeof(int));
  (*e).b[3] = a.b[0];
  assert((*e).b[3] == 0);
  free(e);
}
// Imported from ESBMC regression/esbmc/github_133
// RUN: --force-malloc-success --multi-property
// EXPECT: ^\s*FAILED\s+\[.*\]\s+line\s+[0-9]+\s+array bounds violated: array upper bound$
// EXPECT: ^\s*FAILED\s+\[.*\]\s+line\s+[0-9]+\s+dereference failure: array bounds violated: heap object$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^\s*FAILED\s+\[.*\]\s+line\s+[0-9]+\s+array bounds violated: array upper bound$
// MODE: FUTURE
// Vereda: verdict FAILED
