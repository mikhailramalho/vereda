unsigned __VERIFIER_nondet_uint(void);
int main(void)
{
  unsigned x = __VERIFIER_nondet_uint();
  unsigned i = 0;
  __VERIFIER_assert(!__ESBMC_forall(&i, i < x)); // some i is not below x
  return 0;
}
// Imported from ESBMC regression/goto2lean/quantifier_negated
// Needs: __ESBMC_forall
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^  ¬\(∀ i : BitVec 32, \(i < x\)\) ∧ \($
// EXPECT: any other quantifier needs a manual proof
// EXPECT: ^  sorry$
