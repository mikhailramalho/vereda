unsigned __VERIFIER_nondet_uint(void);

int main(void)
{
  unsigned x = __VERIFIER_nondet_uint();
  unsigned y = -x;

  /* Modular negation: both hold for every unsigned x. Simplifying -x to
   * (2^32 - x) % 2^32 left `(0 - x) % 0` in the SSA, because 2^32 is not
   * representable in the 32-bit type the constant was given. */
  __VERIFIER_assert((unsigned)(x + y) == 0u); // x + (-x) == 0
  __VERIFIER_assert(x != 0u || y == 0u); // -0 == 0
  return 0;
}
// Imported from ESBMC regression/esbmc/github_4625_neg_unsigned_encoding
// Needs: --output neg.smt2 CHECK_FILE neg.smt2 contains bvneg CHECK_FILE neg.smt2 absent bvurem
// RUN: --64 --smt-formula-only --output neg.smt2
// MODE: FUTURE
// EXPECT: ^SMT formula dumped to file
// Vereda: verdict none
