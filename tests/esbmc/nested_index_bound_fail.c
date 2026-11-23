int main()
{
  int a[2][3];
  for (int *p = &a[0][0]; p != &a[2][1]; p++)
    *p = 1;
  return 0;
}
// Imported from ESBMC regression/esbmc/nested_index_bound_fail
// Needs: --no-pointer-arith-check
// RUN: --unwind 8 --no-pointer-arith-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: array bounds violated
// Vereda: verdict none
