/* The ranking proof's measure goes into the correctness witness as a
 * transition invariant on the outer loop; the inner loop restarts on every
 * outer iteration, so its measure alone would not hold and it gets none. */
extern int __VERIFIER_nondet_int(void);

int main()
{
  int x = __VERIFIER_nondet_int();
  int y;
  while (x > 0)
  {
    y = 10;
    while (y > 0)
      y--;
    x--;
  }
  return 0;
}
// Imported from ESBMC regression/termination/ranking_transition_invariant
// Needs: --termination --max-inductive-step 3 --witness-output-yaml
// RUN: --termination --k-step 2 --max-k-step 6 --max-inductive-step 3 --no-assertions --ub-pointer-check=false --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: type: "loop_transition_invariant"
// EXPECT: line: 10$
// EXPECT: value: ".*\\\\at\(x, AnyPrev\).* > .*x.*"$
// EXPECT: \A(?![\s\S]*line: 13$)[\s\S]*\Z
// Vereda: verdict none
