// f() writes g through p, so the return must not put back g's old value.
int g;
int *p = &g;
void f(void)
{
  *p = 5;
}
int main()
{
  g = 0;
  f();
  __VERIFIER_assert(g == 0); // g
}
// Imported from ESBMC regression/esbmc/interval_return_write_through_pointer_fail
// Needs: --interval-analysis
// RUN: --interval-analysis
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
