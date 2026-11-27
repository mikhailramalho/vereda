// Each solving phase of a resumed bound runs on a fresh solver instance, so
// the constraints asserted while walking, here the layout that keeps the
// objects apart, are asserted on it again.
// RUN: --no-summarize-array-loops --incremental-bmc --max-k-step 10
// EXPECT: \[PROGRESS\] Resuming main
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdint.h>
#include <stdlib.h>
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);
int g0 = 1, g1 = 2;
int *table[2];
int main(void) {
  table[0] = &g0;
  table[1] = &g1;
  uintptr_t ap = (uintptr_t)table[__VERIFIER_nondet_int() ? 1 : 0];
  int *q = malloc(sizeof(int));
  int s = 0;
  for (int i = 0; i < 4; ++i)
    s += __VERIFIER_nondet_int() & 1;
  if (q)
    __VERIFIER_assert((uintptr_t)q != ap + (s & 3));
  __VERIFIER_assert(ap != 0);
}
