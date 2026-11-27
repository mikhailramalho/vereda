// Each call puts its operand of && on its own line, and the whole condition
// is a return value, not a branching statement: the return gets no branching
// waypoint. The if that tests the result keeps its own.
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);
int a, b;
int one(void) { return 1; }
int pa(void) { return a > 0; }
int pb(void) { return b > 0; }
int inv(void)
{
  return (
    one() &&
    pa() &&
    one() &&
    pb());
}
int main(void)
{
  a = __VERIFIER_nondet_int();
  b = __VERIFIER_nondet_int();
  if (!inv())
    return 0;
  if (a == 5)
    reach_error();
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10769_return_and_lines_fail
// Needs: --witness-output-yaml
// RUN: --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: (?s)type: "branching"(?:(?!type:).)*?^\s+line: 22$
// EXPECT: (?s)\A(?!.*type: "branching"(?:(?!type:).)*?^\s+line: 12$)
// Vereda: verdict none
