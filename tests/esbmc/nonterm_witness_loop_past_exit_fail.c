// Issue #9886: cleanup's loop sorts first, but main calls cleanup only past
// its own loop's exit, which the inductive step shows unreachable, so the
// cycle waypoint names main's loop.
extern int __VERIFIER_nondet_int(void);
int sum;
void cleanup(void)
{
  int i = 0;
  while (1)
  {
    if (i >= 3)
      break;
    sum = sum + __VERIFIER_nondet_int();
    i = i + 1;
  }
}
int main(void)
{
  int x = __VERIFIER_nondet_int();
  while (x > 0)
    x = 1;
  cleanup();
  return 0;
}
// Imported from ESBMC regression/esbmc/nonterm_witness_loop_past_exit_fail
// Needs: --termination --max-inductive-step 3 --witness-output-yaml -
// RUN: --termination --max-inductive-step 3 --k-step 2 --unlimited-k-steps --no-pointer-check --no-bounds-check --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: non-terminating execution
// EXPECT: action: "cycle"
// EXPECT: line: 20$
// EXPECT: function: "main"
// Vereda: verdict none
