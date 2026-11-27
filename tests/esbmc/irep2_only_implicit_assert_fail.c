int main(void)
{
  int x = 1;
  assert(x == 2);
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_implicit_assert_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
