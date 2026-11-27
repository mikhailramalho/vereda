/* Regression test for nested quantifiers mixing forall and exists.
 * Tests that expand_quantifier_defs_in correctly inlines inner exists
 * bodies when the outer quantifier is forall, and vice versa.
 *
 * All assertions below must pass (VERIFICATION SUCCESSFUL).
 *
 *   exists a: forall b: (a == b)           => false (no single a equals all b) => == 0
 *   exists a: exists b: (a != b)           => true  (a=0, b=1 is a witness)    => == 1
 *   forall a: forall b: (a>b)||(b>=a)      => true  (trichotomy: always holds)  => == 1
 *   forall a: exists b: (b == a)           => true  (pick b = a for any a)      => == 1
 *   exists a: forall b: (a==17)&&(b==17)   => false (forall b fails at b=0)     => == 0
 */
int main()
{
  int a1, b1;

  __VERIFIER_assert(
    __ESBMC_exists(&a1, __ESBMC_forall(&b1, (a1 == b1))) == 0
); // exists a: forall b: a==b should be false

  __VERIFIER_assert(
    __ESBMC_exists(&a1, __ESBMC_exists(&b1, (a1 != b1))) == 1
); // exists a: exists b: a!=b should be true

  __VERIFIER_assert(
    __ESBMC_forall(&a1, __ESBMC_forall(&b1, (a1 > b1) || (b1 >= a1))) == 1
); // forall a: forall b: (a>b)||(b>=a) should be true

  __VERIFIER_assert(
    __ESBMC_forall(&a1, __ESBMC_exists(&b1, (b1 == a1))) == 1
); // forall a: exists b: b==a should be true

  __VERIFIER_assert(
    __ESBMC_exists(&a1, __ESBMC_forall(&b1, (a1 == 17) && (b1 == 17))) == 0
); // exists a: forall b: (a==17)&&(b==17) should be false

  return 0;
}
// Imported from ESBMC regression/z3/github_3886_3
// Needs: __ESBMC_exists, __ESBMC_forall
// RUN: --solver z3 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
