// Issue #9886: the first loop sits in a branch that never runs, and the
// ranking check proves it terminates, so the cycle waypoint names the second.
extern int __VERIFIER_nondet_int(void);
int main(void)
{
  int c = 5, sum = 0;
  if (c > 10)
  {
    int i = 0;
    while (i < 3)
    {
      sum = sum + __VERIFIER_nondet_int();
      i = i + 1;
    }
  }
  int x = __VERIFIER_nondet_int();
  while (x > 0)
    x = 1;
  return sum;
}
// Imported from ESBMC regression/esbmc/nonterm_witness_ranked_loop_fail
// Needs: --termination --max-inductive-step 3 --witness-output-yaml -
// RUN: --termination --max-inductive-step 3 --k-step 2 --unlimited-k-steps --no-pointer-check --no-bounds-check --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: non-terminating execution
// EXPECT: action: "cycle"
// EXPECT: line: 17$
// EXPECT: function: "main"
