int main()
{
  unsigned x = __VERIFIER_nondet_uint() % 10;
  __VERIFIER_assert(x < 10); // x in range
  return 0;
}
// Imported from ESBMC regression/neurosym/incompatible-strategy
// Needs: --neurosym --k-induction --neurosym-prog fake-neurosym.sh
// RUN: --neurosym --k-induction --neurosym-prog fake-neurosym.sh
// MODE: FUTURE
// EXPECT: does not support --k-induction
// Vereda: verdict none
