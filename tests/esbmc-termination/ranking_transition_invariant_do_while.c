/* A do-while loop has no instruction at its `do` to place the witness's
 * transition invariant on, so it gets none. */
extern int __VERIFIER_nondet_int(void);

int main()
{
  int x = __VERIFIER_nondet_int();
  do
  {
    x--;
  } while (x > 0);
  return 0;
}
// Imported from ESBMC regression/termination/ranking_transition_invariant_do_while
// Needs: --termination --max-inductive-step 3 --no-assertions --witness-output-yaml -
// RUN: --termination --k-step 2 --max-k-step 6 --max-inductive-step 3 --no-assertions --no-pointer-check --no-bounds-check --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: \A(?![\s\S]*loop_transition_invariant)[\s\S]*\Z
// Vereda: verdict none
