// f() may call w() through fp, so the return must not put back g.
int g;
void w(void)
{
  g = 5;
}
void r(void)
{
}
void (*fp)(void);
void f(void)
{
  fp();
}
int main()
{
  fp = __VERIFIER_nondet_int() ? w : r;
  g = 0;
  f();
  __VERIFIER_assert(g == 0); // g
}
// Imported from ESBMC regression/esbmc/interval_return_function_pointer_fail
// Needs: --interval-analysis
// RUN: --interval-analysis
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
