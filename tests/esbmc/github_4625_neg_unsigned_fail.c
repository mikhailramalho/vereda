unsigned __VERIFIER_nondet_uint(void);

int main(void)
{
  unsigned x = __VERIFIER_nondet_uint();
  __VERIFIER_assume(x != 0u);
  unsigned y = -x;

  /* False for every x != 0, so the checker must report it: a negation that
   * collapsed to an unconstrained value would let this through. */
  __VERIFIER_assert(y == 0u); // -x is never zero for x != 0
  return 0;
}
// Imported from ESBMC regression/esbmc/github_4625_neg_unsigned_fail
// RUN: -m64 --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
