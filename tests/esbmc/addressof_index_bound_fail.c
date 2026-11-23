// Anti-vacuity twin: folding &a[n] must keep the bound on the object it came
// from. A fold that discarded the symbol would lose this bounds check.
int main(void)
{
  int a[4] = {1, 1, 1, 1};
  int sum = 0;

  for (int *p = a; p != &a[5]; ++p)
    sum += *p;

  return sum;
}
// Imported from ESBMC regression/esbmc/addressof_index_bound_fail
// Needs: --no-pointer-arith-check
// RUN: --unwind 6 --no-pointer-arith-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: array bounds violated
// Vereda: verdict none
