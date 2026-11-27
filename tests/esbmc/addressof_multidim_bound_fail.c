// Anti-vacuity twin: linearising &a[i][j] must keep the bound on the object it
// came from. A fold that discarded the symbol would lose this bounds check.
int main(void)
{
  int a[2][3] = {{1, 2, 3}, {4, 5, 6}};
  int sum = 0;

  for (int *p = &a[0][0]; p != &a[2][1]; ++p)
    sum += *p;

  return sum;
}
// Imported from ESBMC regression/esbmc/addressof_multidim_bound_fail
// Needs: --no-pointer-arith-check
// RUN: --no-pointer-arith-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: dereference failure: array bounds violated
// Vereda: verdict none
