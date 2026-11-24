int main()
{
  char idx;
  int prop = __ESBMC_forall(&idx, idx == idx + 1);
  __VERIFIER_assert(prop); // prop
  return 0;
}
// Imported from ESBMC regression/z3/quantifiers-noncast-fail
// Needs: __ESBMC_forall
// RUN: --solver z3 --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
