// Each bound reports the first loop cut short, as under --incremental-solver,
// so the bounds tried do not depend on the solver's model.
// RUN: --incremental-bmc --max-k-step 4
// EXPECT: Bound 1 too small: unwinding assertion loop 0$
// EXPECT: Bound 2 too small: unwinding assertion loop 0$
// EXPECT: Bound 3 too small: unwinding assertion loop 0$
// EXPECT-NOT: loop 1$
#include <stdlib.h>
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int n = __VERIFIER_nondet_int();
  int s = 0;
  if (n > 10)
    while (s < n)
      s += 3;
  int *p = malloc(sizeof(int));
  for (int j = 0; j < 9; ++j)
    s -= j;
  free(p);
  return s;
}
