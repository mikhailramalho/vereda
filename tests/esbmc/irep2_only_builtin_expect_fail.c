int main(void)
{
  int x = 1;
  __VERIFIER_assert(__builtin_expect(x == 2, 1)); // expect keeps a false argument false
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_builtin_expect_fail
// EXPECT: ^VERIFICATION FAILED$
