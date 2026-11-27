// No nondet visit of line 8 follows f(0), so no later waypoint can bind to
// it and the fixed visit gets none (#9689).
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);

int f(int m)
{
  if (m < 10)
    return 0;
  return 1;
}

int main(void)
{
  int x = __VERIFIER_nondet_int();
  int b = f(x);
  int a = f(0);
  if (a + b != 0)
    reach_error();
  return 0;
}
// Imported from ESBMC regression/esbmc/witness_fixed_branch_after_nondet_fail
// Needs: --witness-output-yaml
// RUN: --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: (?s)\A(?:(?!line: 8\n).)*value: "false"\n\s+location:\n\s+file_name: "[^"]*witness_fixed_branch_after_nondet_fail\.c"\n\s+line: 8\n(?:(?!line: 8\n).)*\Z
// Vereda: verdict none
