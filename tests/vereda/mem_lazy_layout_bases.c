// Lazy bases materialization and on-demand layout constraints: pointers stored
// in memory and loaded back through a symbolic index still observe distinct,
// aligned, non-zero base addresses, and subsequent allocations laid out after
// bases is materialised remain disjoint from earlier objects.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdint.h>
#include <stdlib.h>

extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);

int g0 = 1, g1 = 2, g2 = 3;
int *table[2];

int main(void) {
  table[0] = &g0;
  table[1] = &g1;
  int idx = __VERIFIER_nondet_int() ? 1 : 0;
  int *p = table[idx];
  uintptr_t ap = (uintptr_t)p;
  __VERIFIER_assert(ap != 0);
  __VERIFIER_assert((ap & (_Alignof(int) - 1)) == 0);
  int *q = malloc(sizeof(int));
  if (q) {
    *q = 99;
    uintptr_t aq = (uintptr_t)q;
    __VERIFIER_assert(aq != ap);
    __VERIFIER_assert(*p == (idx ? 2 : 1));
    __VERIFIER_assert(g2 == 3);
  }
  return 0;
}
