// status takes three values; widening at its first change lost status <= 2.
extern int __VERIFIER_nondet_int(void);
int main()
{
  int status = 0;
  while (__VERIFIER_nondet_int())
  {
    if (!status)
      status = 1;
    else if (status == 1)
      status = 2;
  }
  while (1)
    __VERIFIER_assert(status != 2); // two
}
// Imported from ESBMC regression/k-induction/interval_widening_delay_fail
// Needs: --k-induction --interval-analysis --interval-analysis-arithmetic --interval-analysis-extrapolate --interval-analysis-extrapolate-under-approximate
// RUN: --k-induction --interval-analysis --interval-analysis-arithmetic --interval-analysis-extrapolate --interval-analysis-extrapolate-under-approximate
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
