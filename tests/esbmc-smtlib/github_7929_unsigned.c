/* github #7929: the counterpart of the signed reads. An unsigned value with
 * the top bit set must keep its magnitude; reading it as two's complement
 * only shows up once the value reaches the simplifier, since an out-of-range
 * constant still prints correctly. */
unsigned int __VERIFIER_nondet_uint(void);

int main(void)
{
  unsigned int u = __VERIFIER_nondet_uint();
  __VERIFIER_assume(u == (unsigned int)-4);
  unsigned int q = u / 3u;
  __VERIFIER_assert(q == 9); // cex
  return 0;
}
// Imported from ESBMC regression/smtlib/github_7929_unsigned
// Needs: --smtlib --smtlib-solver-prog z3 -in REQUIRES z3_binary
// RUN: --smtlib --smtlib-solver-prog "z3 -in"
// MODE: FUTURE
// EXPECT: ^  u = 4294967292;\n  bits: 11111111 11111111 11111111 11111100$
// EXPECT: ^  q = 1431655764;\n  bits: 01010101 01010101 01010101 01010100$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
