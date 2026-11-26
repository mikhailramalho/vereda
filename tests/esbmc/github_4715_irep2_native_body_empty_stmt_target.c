// convert() leaves a SKIP at the statement's own location when a statement
// emits nothing into a still-empty program; convert_native_rec did not, and the
// switch-case and label arms need an instruction for their target to sit on, so
// both declined. `assert` under --no-assertions is the one native kind that
// emits nothing -- the same asymmetry that aborted compute_target_numbers
// before the if-arm's else guard (W1-loc, esbmc/esbmc#4715, docs §28).
extern int nd(void);
int main(void)
{
  int c = nd();
  switch (c)
  {
  case 1:
    __VERIFIER_assert(0); // in a case
    break;
  }
  if (c == 2)
    goto L;
  return 0;
L:
  __VERIFIER_assert(0); // under a label
  return 1;
}
// Imported from ESBMC regression/esbmc/github_4715_irep2_native_body_empty_stmt_target
// RUN: --unwind 2 --no-assertions
// EXPECT: ^VERIFICATION SUCCESSFUL$
