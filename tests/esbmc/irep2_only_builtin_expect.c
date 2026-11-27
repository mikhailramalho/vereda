int main(void)
{
  int x = 1;
  __VERIFIER_assert(__builtin_expect(x == 1, 0)); // expect yields its first argument
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_builtin_expect
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
