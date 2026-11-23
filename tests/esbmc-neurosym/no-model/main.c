int main()
{
  unsigned x = __VERIFIER_nondet_uint() % 10;
  __VERIFIER_assert(x < 5); // x less than five
  return 0;
}
// Imported from ESBMC regression/neurosym/no-model
// Needs: --neurosym --neurosym-prog fake-neurosym.sh
// RUN: --neurosym --neurosym-prog fake-neurosym.sh
// MODE: FUTURE
// EXPECT: requires a local interactive SMT-LIB2 solver
// Vereda: verdict none
