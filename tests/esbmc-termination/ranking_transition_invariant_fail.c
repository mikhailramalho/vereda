/* Counterpart of ranking_transition_invariant: the loop does not terminate,
 * so the witness is a violation witness with no transition invariant. */
extern int __VERIFIER_nondet_int(void);

int main()
{
  int x = __VERIFIER_nondet_int();
  int y;
  while (x > 0)
  {
    y = 10;
    x = 1;
  }
  return 0;
}
// Imported from ESBMC regression/termination/ranking_transition_invariant_fail
// Needs: --termination --max-inductive-step 3 --no-assertions --witness-output-yaml -
// RUN: --termination --k-step 2 --max-k-step 6 --max-inductive-step 3 --no-assertions --no-pointer-check --no-bounds-check --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \A(?![\s\S]*loop_transition_invariant)[\s\S]*\Z
