// A loop condition || with calls is split into one goto per operand.
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);
int neg(int x) { return x < 0; }
int odd(int x) { return x % 2 != 0; }
int main(void)
{
  int x = __VERIFIER_nondet_int();
  if (x < -4 || x > 4)
    return 0;
  int n = 0;
  while (neg(x) ||
         odd(x))
  {
    x = x + 1;
    n = n + 1;
  }
  if (n == 2)
    reach_error();
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10769_while_or_lines_fail
// Needs: --witness-output-yaml
// RUN: --unwind 10 --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: (?s)type: "branching"(?:(?!type:).)*?^\s+line: 18$
// EXPECT: (?s)\A(?!.*type: "branching"(?:(?!type:).)*?^\s+line: 1[23]$)
// Vereda: verdict none
