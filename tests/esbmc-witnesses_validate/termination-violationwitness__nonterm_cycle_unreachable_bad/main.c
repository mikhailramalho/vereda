// Issue #9887: the cycle waypoint must lie on a loop that can run forever.
extern int __VERIFIER_nondet_int(void);
int drain(void)
{
  int x = 1;
  while (x)
    x = __VERIFIER_nondet_int();
  return x;
}
int main(void)
{
  int x = __VERIFIER_nondet_int();
  while (x > 0)
    x = 1;
  return 0;
}
// Imported from ESBMC regression/witnesses_validate/termination-violationwitness/nonterm_cycle_unreachable_bad
// Needs: --termination --max-inductive-step 3 --k-step 2 --unlimited-k-steps --validate-violation-witness --witness witness.yml
// RUN: --termination --max-inductive-step 3 --k-step 2 --unlimited-k-steps --no-pointer-check --no-bounds-check --validate-violation-witness --witness witness.yml
// MODE: FUTURE
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none
