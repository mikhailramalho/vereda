unsigned __VERIFIER_nondet_uint(void);
int main(void)
{
  unsigned x = __VERIFIER_nondet_uint();
  unsigned i = 0, j = 0;
  __VERIFIER_assume(x < 10);
  __VERIFIER_assert(__ESBMC_forall(&i, i + x >= i || i > 4294967285u)); // no wrap
  __VERIFIER_assert(
    __ESBMC_forall(&i, i <= 4294967295u) && __ESBMC_forall(&j, j >= x || j < 10)
); // conjunction
  return 0;
}
// Imported from ESBMC regression/goto2lean/quantifier_forall
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^  \(∀ i : BitVec 32, \(\(i ≤ \(i \+ x\)\) ∨ \(\(4294967285#32\) < i\)\)\) ∧ \($
// EXPECT: ^  all_goals bv_decide$
// Vereda: verdict none
