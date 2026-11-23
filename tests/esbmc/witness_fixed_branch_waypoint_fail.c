// The first call's branch is fixed by the program text, the second's reads
// x. A validator matches the "true" waypoint to the next execution of line 9,
// the first call, unless that call has a "false" waypoint of its own.
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);

void __VERIFIER_assert(int cond)
{
  if (!cond)
    reach_error();
}

int main(void)
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assert(1);
  __VERIFIER_assert(x != 3);
  return 0;
}
// Imported from ESBMC regression/esbmc/witness_fixed_branch_waypoint_fail
// Needs: --witness-output-yaml -
// RUN: --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: (?s)value: "false"\n\s+location:\n\s+file_name: "[^"]*witness_fixed_branch_waypoint_fail\.c"\n\s+line: 9$.*value: "true"\n\s+location:\n\s+file_name: "[^"]*witness_fixed_branch_waypoint_fail\.c"\n\s+line: 9$
// Vereda: verdict none
