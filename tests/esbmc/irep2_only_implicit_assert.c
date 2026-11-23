/* No <assert.h>: `assert` is implicitly declared, so the adjuster declares the
   callee itself and must give the symbol the base name goto_convert matches. */
int main(void)
{
  int x = 1;
  assert(x == 1);
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_implicit_assert
// Needs: --goto-functions-only --clang-c-irep2-adjust-only
// RUN: --goto-functions-only --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: ASSERT x == 1
// Vereda: verdict none
