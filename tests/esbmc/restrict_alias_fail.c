/* Two restrict-qualified pointer parameters that alias while both are written
   are undefined behaviour (C11 6.7.3.1). --restrict-check must catch this. */
void f(int *restrict a, int *restrict b)
{
  *a = 1;
  *b = 2;
}

int main(void)
{
  int x = 0;
  f(&x, &x);
  return 0;
}
// Imported from ESBMC regression/esbmc/restrict_alias_fail
// Needs: --restrict-check
// RUN: --restrict-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\s*restrict pointer aliasing$
// Vereda: verdict none
