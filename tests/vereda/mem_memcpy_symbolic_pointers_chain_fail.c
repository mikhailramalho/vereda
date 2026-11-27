// Pointers copied twice by memcpy of nondet size into objects without
// arrays of their own, table to p to q, are q's too: a call without a body
// that gets q may follow them and write x.
// RUN: --force-malloc-success
// EXPECT: ^VERIFICATION FAILED$
#include <stdlib.h>
#include <string.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assert(int);
extern void ext(int **);
int x = 1;
int *table[300];

int main(void) {
  unsigned n = __VERIFIER_nondet_uint();
  table[0] = &x;
  int **p = malloc(sizeof table), **q = malloc(sizeof table);
  memcpy(p, table, n);
  memcpy(q, p, n);
  ext(q);
  __VERIFIER_assert(x == 1);
  return 0;
}
