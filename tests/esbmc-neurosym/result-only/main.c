int main()
{
  unsigned x = __VERIFIER_nondet_uint() % 10;
  __VERIFIER_assert(x < 5); // x less than five
  return 0;
}
// Imported from ESBMC regression/neurosym/result-only
// Needs: --neurosym --neurosym-prog fake-neurosym.sh --result-only
// RUN: --neurosym --neurosym-prog fake-neurosym.sh --result-only
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
