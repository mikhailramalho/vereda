int ext(void);
void f(void)
{
}
int main()
{
  int x = 3, y = 0;
  f();
  y = ext();
  __VERIFIER_assert(x == 3); // x
}
// Imported from ESBMC regression/esbmc/interval_return_bodiless_call
// Needs: --interval-analysis
// RUN: --interval-analysis
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
