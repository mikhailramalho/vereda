/* Neither y - x nor 2147483647 - z falls on every iteration, but the pair
 * falls lexicographically: the transition invariant is a disjunction with
 * one disjunct per measure. */
extern int __VERIFIER_nondet_int(void);

int main()
{
  int x = __VERIFIER_nondet_int();
  int y = __VERIFIER_nondet_int();
  int z = __VERIFIER_nondet_int();
  while (x < y && z < 2147483647)
  {
    if (x < z)
      x++;
    else
      z++;
  }
  return 0;
}
// Imported from ESBMC regression/termination/ranking_transition_invariant_lex
// Needs: --termination --k-step 2 --max-inductive-step 3 --no-assertions --witness-output-yaml -
// RUN: --termination --k-step 2 --max-k-step 6 --max-inductive-step 3 --no-assertions --no-pointer-check --no-bounds-check --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: type: "loop_transition_invariant"
// EXPECT: line: 11$
// EXPECT: value: ".*AnyPrev.* > .* \|\| .*AnyPrev.* > .*"$
// Vereda: verdict none
