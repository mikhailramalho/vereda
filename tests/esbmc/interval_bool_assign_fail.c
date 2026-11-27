_Bool flag = 0;

int main()
{
  flag = __VERIFIER_nondet_bool();
  if (!flag)
  {
    flag = 1;
    __VERIFIER_assert(!flag); // stale interval fact must not survive the write
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/interval_bool_assign_fail
// Needs: --interval-analysis
// RUN: --interval-analysis --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
