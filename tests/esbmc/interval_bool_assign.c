int main()
{
  int x = __VERIFIER_nondet_int();
  _Bool b;

  if (x > 5)
  {
    b = __VERIFIER_nondet_bool();
    __VERIFIER_assert(x > 0); // an unrelated interval fact survives an unmodelled write
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/interval_bool_assign
// Needs: --interval-analysis
// RUN: --interval-analysis --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
