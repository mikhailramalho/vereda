int main()
{
  unsigned x = __VERIFIER_nondet_uint() % 10;
  __VERIFIER_assert(x < 10); // x in range
  return 0;
}
// Imported from ESBMC regression/neurosym/basic-safe
// Needs: --neurosym --neurosym-prog fake-neurosym.sh
// RUN: --neurosym --neurosym-prog fake-neurosym.sh
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
