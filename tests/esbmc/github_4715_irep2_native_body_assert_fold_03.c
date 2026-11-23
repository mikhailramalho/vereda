// generate_ifthenelse gates the `(cond) || (assert(0),0)` fold on the else
// *program* being observationally no-op, not on there being no else -- the
// native arm read the AST instead and missed this shape. The fold now also
// needs the discarded instruction to be a no-op; `g = 1` is not, so both arms
// keep the branch (esbmc/esbmc#7900).
extern int nd(void);
int g;
int main(void)
{
  int c = nd();
  if (c == 2)
  {
    __VERIFIER_assert(0); // j
    g = 1;
  }
  else
  {
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/github_4715_irep2_native_body_assert_fold_03
// Needs: --goto-functions-only
// RUN: --goto-functions-only --multi-property
// MODE: FUTURE
// EXPECT: ^\s+IF !\(c == 2\) THEN GOTO \d+$
// EXPECT: ^\s+ASSERT 0 // j$
// EXPECT: ^\s+ASSIGN g=1;$
// Vereda: verdict none
