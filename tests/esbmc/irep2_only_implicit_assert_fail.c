int main(void)
{
  int x = 1;
  assert(x == 2);
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_implicit_assert_fail
// Needs: --clang-c-irep2-adjust-only
// RUN: --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
