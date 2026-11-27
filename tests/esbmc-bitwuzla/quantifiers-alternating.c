// Bitwuzla used to time out on a genuine forall/exists *alternation* -- exists
// nested inside forall -- while Z3 discharged it immediately (#4033). The
// neighbouring quantifiers-hello only asserts the two quantifiers separately,
// which is the easy case, so pin the alternation itself.
int main()
{
  int a, b;
  __VERIFIER_assert(
    __ESBMC_forall(&a, __ESBMC_exists(&b, b == a + 1))
); // for every a there exists b = a+1
  return 0;
}
// Imported from ESBMC regression/bitwuzla/quantifiers-alternating
// Needs: __ESBMC_exists, __ESBMC_forall
// RUN: --solver bitwuzla --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
