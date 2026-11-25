/* github #7929: same root cause reached through arithmetic -- INT_MIN read as
 * 2147483648 makes the remainder fold to a value outside the type. */
int __VERIFIER_nondet_int(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  int y = __VERIFIER_nondet_int();
  __VERIFIER_assume(x == -2147483647 - 1);
  __VERIFIER_assume(y == -104635056);
  int r = x % y;
  __VERIFIER_assert(r == 9); // cex
  return 0;
}
// Imported from ESBMC regression/smtlib/github_7929_rem
// Needs: --smtlib --smtlib-solver-prog z3 -in REQUIRES z3_binary
// RUN: --smtlib --smtlib-solver-prog "z3 -in"
// MODE: FUTURE
// EXPECT: ^  r = -54782528;\n  bits: 11111100 10111100 00010101 11000000$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
