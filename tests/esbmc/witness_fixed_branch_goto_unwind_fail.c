// --goto-unwind copies the loop body, so each iteration's `if` is its own
// instruction. The first iteration's branch is fixed (c is 0) and still needs
// a "false" waypoint, or a validator binds the second iteration's "true" to it.
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  for (int i = 0; i < 2; i++)
  {
    int c = i ? x : 0;
    if (c == 3)
      reach_error();
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/witness_fixed_branch_goto_unwind_fail
// Needs: --witness-output-yaml -
// RUN: --witness-output-yaml -
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: (?s)value: "false"\n\s+location:\n\s+file_name: "[^"]*witness_fixed_branch_goto_unwind_fail\.c"\n\s+line: 13$.*value: "true"\n\s+location:\n\s+file_name: "[^"]*witness_fixed_branch_goto_unwind_fail\.c"\n\s+line: 13$
// MODE: FUTURE
// Vereda: verdict none
