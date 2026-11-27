// f() writes g only through h(), so the return from f() must not put back g.
int g;
void h(void)
{
  g = 5;
}
void f(void)
{
  h();
}
int main()
{
  g = 0;
  f();
  __VERIFIER_assert(g == 0); // g
}
// Imported from ESBMC regression/esbmc/interval_return_global_nested_write_fail
// Needs: --interval-analysis
// RUN: --interval-analysis --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
