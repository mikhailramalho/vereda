// #9222: a signed product above INT_MAX is an overflow.
int __VERIFIER_nondet_int(void);
int main(void)
{
  int a = __VERIFIER_nondet_int(), b = __VERIFIER_nondet_int();
  __VERIFIER_assume((a < 0 && b < 0) || (a > 0 && b > 0));
  return a * b;
}
// Imported from ESBMC regression/z3/github_9222_pos_fail
// RUN: --solver z3 --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check
// EXPECT: arithmetic overflow on mul
// EXPECT: ^VERIFICATION FAILED$
