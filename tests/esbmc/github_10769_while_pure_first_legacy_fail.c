// The loop's first instruction is the goto of the pure operand x > 0, whose
// location convert_while resets to the statement's; it must stay an operand.
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);
int pb(int x) { return x < 5; }
int main(void)
{
  int x = __VERIFIER_nondet_int();
  int n = 0;
  while (x > 0 &&
         pb(x))
  {
    x = x + 1;
    n = n + 1;
  }
  if (n == 2)
    reach_error();
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10769_while_pure_first_legacy_fail
// Needs: --no-irep2-native-body --witness-output-yaml -
// RUN: --unwind 6 --no-irep2-native-body --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: (?s)type: "branching"(?:(?!type:).)*?^\s+line: 16$
// EXPECT: (?s)\A(?!.*type: "branching"(?:(?!type:).)*?^\s+line: 1[01]$)
// Vereda: verdict none
