// The write through p sets i, whose address is taken, so the step must not
// keep i's lower bound 1: i * 3 overflows on the sixth iteration.
int __VERIFIER_nondet_int(void);

int main()
{
  int n = __VERIFIER_nondet_int();
  if (n < 0)
    return 0;
  int a = 0;
  int i = 1;
  int *p = &i;
  while (i <= n / 3)
  {
    a = i * 3;
    if (i == 5)
      *p = -2147483647;
    else
      i = i + 1;
  }
  return a;
}
// Imported from ESBMC regression/k-induction/interval_pointer_write_fail
// Needs: --k-induction --max-inductive-step 3 --interval-analysis --interval-analysis-arithmetic --interval-analysis-extrapolate --interval-analysis-extrapolate-under-approximate --interval-analysis-assume-asserts
// RUN: --k-induction --max-inductive-step 3 --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --interval-analysis --interval-analysis-arithmetic --interval-analysis-extrapolate --interval-analysis-extrapolate-under-approximate --interval-analysis-assume-asserts
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
