// generate_ifthenelse folds a branch that reduces to a lone assert(false) into
// the guard rather than emitting a conditional GOTO. The native if-arm used to
// decline these shapes, taking the whole function to the round-trip; it now
// reproduces the fold (W1-loc, esbmc/esbmc#4715). One case per fold branch:
// then-only, then-only in a block, else-only, both, and then-only with a
// no-op else -- the last is why the fold tests the branch program, not the AST.
extern int nd(void);
int main(void)
{
  int c = nd();
  if (c == 1)
    __VERIFIER_assert(0); // a
  if (c == 2)
  {
    __VERIFIER_assert(0); // b
  }
  if (c == 3)
    ;
  else
    __VERIFIER_assert(0); // c
  if (c == 4)
    __VERIFIER_assert(0); // d
  else
    __VERIFIER_assert(0); // e
  if (c == 5)
    __VERIFIER_assert(0); // f
  else
    ;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_4715_irep2_native_body_assert_fold_01
// Needs: --goto-functions-only
// RUN: --goto-functions-only
// MODE: FUTURE
// EXPECT: ASSERT !\(c == 1\) // a
// EXPECT: ASSERT !\(c == 2\) // b
// EXPECT: ASSERT c == 3 // c
// EXPECT: ASSERT !\(c == 4\) // d
// EXPECT: ASSERT c == 4 // e
// EXPECT: ASSERT !\(c == 5\) // f
// Vereda: verdict none
