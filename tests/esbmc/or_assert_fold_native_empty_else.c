/* The native arm folds `(c) || (assert(0), 0)` with an empty else, provided the
   discarded statement is a no-op (esbmc/esbmc#7900). */
extern int nd(void);
int main(void)
{
  int c = nd();
  if (c == 2)
  {
    __VERIFIER_assert(0); // j
    (void)0;
  }
  else
  {
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/or_assert_fold_native_empty_else
// Needs: --goto-functions-only
// RUN: --goto-functions-only
// MODE: FUTURE
// EXPECT: ^\s+ASSERT !\(c == 2\) // j$
// EXPECT: \A(?![\s\S]*ASSERT 0 // j)[\s\S]*\Z
// Vereda: verdict none
