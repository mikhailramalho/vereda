int main()
{
  int a, b;
  /* forall a b: (a == b) || (b == 17) — false when a=0, b=0 */
  __VERIFIER_assert(
    __ESBMC_forall(&a, __ESBMC_forall(&b, (a == b) || (b == 17))) == 0
); // nested forall should be false
  return 0;
}
// Imported from ESBMC regression/bitwuzla/quantifiers-nested
// Needs: __ESBMC_forall
// RUN: --solver bitwuzla --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
