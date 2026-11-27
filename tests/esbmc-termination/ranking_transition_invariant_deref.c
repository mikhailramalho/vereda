/* The measure reads *p, and \at(p, AnyPrev) would name the old pointer
 * rather than the old value: the witness gets no transition invariant. */
extern int __VERIFIER_nondet_int(void);

int main()
{
  int x = __VERIFIER_nondet_int();
  int *p = &x;
  while (*p > 0)
    (*p)--;
  return 0;
}
// Imported from ESBMC regression/termination/ranking_transition_invariant_deref
// Needs: --termination --max-inductive-step 3 --witness-output-yaml
// RUN: --termination --k-step 2 --max-k-step 6 --max-inductive-step 3 --no-assertions --ub-pointer-check=false --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: \A(?![\s\S]*loop_transition_invariant)[\s\S]*\Z
// Vereda: verdict none
