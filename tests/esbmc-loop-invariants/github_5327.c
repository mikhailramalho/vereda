/* The two assumes contradict, so every auto-generated overflow check below
 * them is discharged on a dead path. That is the intended safe outcome -- the
 * overflow genuinely cannot happen -- so the vacuity probe must not downgrade
 * the verdict on account of it. */
extern int __VERIFIER_nondet_int(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x > 0);
  __VERIFIER_assume(x < 0);

  int i = 0;
  __ESBMC_loop_invariant(i <= 3);
  while (i < 3)
  {
    i = i + 1;
  }

  int y = x + 1;
  return y;
}
// Imported from ESBMC regression/loop-invariants/github_5327
// Needs: --loop-invariant-check
// RUN: --overflow-check --loop-invariant-check --unwind 5
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: (?s)\A(?!.*WARNING: Vacuous discharge)
// Vereda: verdict none
