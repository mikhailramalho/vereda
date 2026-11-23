// The return from f() put back y's value from before the call after ext()
// had already forgotten it.
int ext(void);
void f(void)
{
}
int main()
{
  int y = 0;
  f();
  y = ext();
  __VERIFIER_assert(y == 0); // y
}
// Imported from ESBMC regression/esbmc/interval_return_bodiless_call_fail
// Needs: --interval-analysis
// RUN: --interval-analysis
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
