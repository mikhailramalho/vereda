unsigned __VERIFIER_nondet_uint(void);
int main(void)
{
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n > 0 && n < 4);
  return (int)sizeof(int[n]);
}
// Imported from ESBMC regression/esbmc/migrate_level0_symbol_quiet
// Needs: --goto-functions-only
// RUN: --goto-functions-only
// EXPECT: (?s)\A(?!.*missing renaming delimiters)
