int zero_array[10];

int main()
{
  int sym;
  __VERIFIER_assert(
    __ESBMC_forall(&sym, !(sym >= 0 && sym < 10) || zero_array[sym] == 0)
); // array is zero initialized

  const unsigned N = 10;
  unsigned i = 0;
  char c[N];

  for (i = 0; i < N; ++i)
    c[i] = i;

  unsigned j;
  __VERIFIER_assert(
    __ESBMC_forall(&i, i > 9 || c[i] == i)); // array is initialized correctly

  __VERIFIER_assert(__ESBMC_exists(&i, i == 0)); // can be anything
  __VERIFIER_assert(__ESBMC_exists(&i, i == 1)); // can be anything 2
  __VERIFIER_assert(!__ESBMC_exists(&i, i == 0 && i == 1)); // contradiction
}
// Imported from ESBMC regression/cvc/quantifiers-bug
// Needs: __ESBMC_exists, __ESBMC_forall
// RUN: --solver cvc5 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
