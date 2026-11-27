// The write through p cannot change i, whose address is never taken, so i
// keeps its lower bound 1 and i * 3 cannot overflow.
#include <stdlib.h>
int __VERIFIER_nondet_int(void);

int main()
{
  int n = __VERIFIER_nondet_int();
  if (n < 0)
    return 0;
  int *p = malloc(sizeof(int));
  if (!p)
    return 0;
  for (int i = 1; i <= n / 3; i++)
    *p = i * 3;
  free(p);
  return 0;
}
// Imported from ESBMC regression/k-induction/interval_pointer_write
// Needs: --k-induction --max-inductive-step 3 --interval-analysis --interval-analysis-arithmetic --interval-analysis-extrapolate --interval-analysis-extrapolate-under-approximate --interval-analysis-assume-asserts
// RUN: --k-induction --max-inductive-step 3 --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --interval-analysis --interval-analysis-arithmetic --interval-analysis-extrapolate --interval-analysis-extrapolate-under-approximate --interval-analysis-assume-asserts
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
