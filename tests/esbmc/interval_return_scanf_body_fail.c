// Symex runs its scanf model whatever the body, so the return must not put
// back x's old value.
int scanf(const char *fmt, ...)
{
  return 1;
}
int main()
{
  int x = 0;
  scanf("%d", &x);
  __VERIFIER_assert(x == 0); // x
}
// Imported from ESBMC regression/esbmc/interval_return_scanf_body_fail
// Needs: --interval-analysis
// RUN: --interval-analysis
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
