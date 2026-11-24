unsigned __VERIFIER_nondet_uint(void);
int main(void) {
  unsigned x = __VERIFIER_nondet_uint();
  unsigned i = 0;
  __VERIFIER_assert(__ESBMC_exists(&i, i > x || x == 4294967295u)); // some larger
  __VERIFIER_assert(__ESBMC_forall(&i, i <= 4294967295u)); // trivial
  return 0;
}
// Imported from ESBMC regression/goto2lean/quantifiers
// Needs: __ESBMC_exists, __ESBMC_forall
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^  \(∃ i : BitVec 32, \(\(x < i\) ∨ \(x = \(4294967295#32\)\)\)\) ∧ \($
// EXPECT: ^  \(∀ i : BitVec 32, \(i ≤ \(4294967295#32\)\)\) ∧ \($
// EXPECT: any other quantifier needs a manual proof
// EXPECT: ^  sorry$
