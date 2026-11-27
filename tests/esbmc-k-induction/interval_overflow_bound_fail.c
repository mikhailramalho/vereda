// x + 1 overflows on the eighth iteration. The bound x <= INT_MAX - 1 taken
// from its overflow check must not let the inductive step prove it first.
int __VERIFIER_nondet_int(void);

int main()
{
  int x = 2147483640;
  while (__VERIFIER_nondet_int())
    x = x + 1;
  return x;
}
// Imported from ESBMC regression/k-induction/interval_overflow_bound_fail
// Needs: --k-induction --max-inductive-step 3 --interval-analysis --interval-analysis-arithmetic --interval-analysis-extrapolate --interval-analysis-extrapolate-under-approximate --interval-analysis-assume-asserts
// RUN: --k-induction --max-inductive-step 3 --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --interval-analysis --interval-analysis-arithmetic --interval-analysis-extrapolate --interval-analysis-extrapolate-under-approximate --interval-analysis-assume-asserts
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
