// An || whose operands call functions, assigned rather than branched on.
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);
int neg(int x) { return x < 0; }
int big(int x) { return x > 100; }
int main(void)
{
  int x = __VERIFIER_nondet_int();
  int out = neg(x) ||
            big(x);
  if (out)
    reach_error();
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10769_assign_or_fail
// Needs: --witness-output-yaml -
// RUN: --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: (?s)type: "branching"(?:(?!type:).)*?^\s+line: 11$
// EXPECT: (?s)\A(?!.*type: "branching"(?:(?!type:).)*?^\s+line: 9$)
// Vereda: verdict none
