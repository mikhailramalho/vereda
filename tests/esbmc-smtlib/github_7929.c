/* github #7929: the smtlib backend read a negative bitvector value out of the
 * model as an unsigned magnitude, so get() folded (a < 0) to false. */
int __VERIFIER_nondet_int(void);

int main(void)
{
  int a = __VERIFIER_nondet_int();
  __VERIFIER_assume(a == -4);
  int f = (a < 0);
  __VERIFIER_assert(f == 9); // cex
  return 0;
}
// Imported from ESBMC regression/smtlib/github_7929
// Needs: --smtlib --smtlib-solver-prog z3 -in REQUIRES z3_binary
// RUN: --smtlib --smtlib-solver-prog "z3 -in"
// MODE: FUTURE
// EXPECT: ^  a = -4;\n  bits: 11111111 11111111 11111111 11111100$
// EXPECT: ^  f = 1;\n  bits: 00000000 00000000 00000000 00000001$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
