// #9222: a signed product below INT_MIN is an overflow.
int __VERIFIER_nondet_int(void);
int main(void)
{
  int a = __VERIFIER_nondet_int(), b = __VERIFIER_nondet_int();
  __VERIFIER_assume(a > 0 && b < 0);
  return a * b;
}
// Imported from ESBMC regression/z3/github_9222_fail
// RUN: --solver z3 --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check
// EXPECT: ^\s+a = [0-9]+
// EXPECT: ^\s+b = -[0-9]+
// EXPECT: arithmetic overflow on mul
// EXPECT: ^VERIFICATION FAILED$
