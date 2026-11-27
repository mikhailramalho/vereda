// Only b decides the violation, so the store at line 10 is kept for its
// function_return waypoint alone; its index is free, and the trace must not
// show a store the program never makes.
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);
int a[4] = {1, 1, 1, 1};
int main(void)
{
  int i = (__VERIFIER_nondet_int() & 1) + 2;
  a[i] = __VERIFIER_nondet_int();
  int b = __VERIFIER_nondet_int();
  if (b == 5)
    reach_error();
  return 0;
}
// Imported from ESBMC regression/esbmc/witness_kept_nondet_call_hidden_fail
// Needs: --witness-output-yaml
// RUN: --witness-output-yaml - --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: value: "\\\\result == -?\d+"\n\s+format: "ext_c_expression"\n\s+location:\n\s+file_name: "[^"]*witness_kept_nondet_call_hidden_fail\.c"\n\s+line: 10$
// EXPECT: (?s)\A(?:(?!\n  a\[).)*\Z
// Vereda: verdict none
