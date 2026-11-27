// Issue #9657: a non-terminating loop yields a YAML cycle waypoint.
extern int __VERIFIER_nondet_int(void);
int main(void)
{
  int x = __VERIFIER_nondet_int();
  while (x > 0)
    x = 1;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9657_nonterm_witness_fail
// Needs: --termination --max-inductive-step 3 --witness-output-yaml
// RUN: --termination --max-inductive-step 3 --k-step 2 --unlimited-k-steps --ub-pointer-check=false --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: non-terminating execution
// EXPECT: action: "cycle"
// EXPECT: line: 6$
// EXPECT: column: 3$
// EXPECT: specification: "F end"
// Vereda: verdict none
