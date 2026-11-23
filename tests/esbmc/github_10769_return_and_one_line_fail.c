// The && in a return lowers to a single conditional goto, which is still an
// operand of the condition, not a branching statement.
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);
int pos(int x) { return x > 0; }
int small(int x) { return x < 10; }
int valid(int x) { return pos(x) && small(x); }
int main(void)
{
  int x = __VERIFIER_nondet_int();
  if (valid(x))
    reach_error();
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10769_return_and_one_line_fail
// Needs: --witness-output-yaml -
// RUN: --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: (?s)type: "branching"(?:(?!type:).)*?^\s+line: 11$
// EXPECT: (?s)\A(?!.*type: "branching"(?:(?!type:).)*?^\s+line: 7$)
// Vereda: verdict none
