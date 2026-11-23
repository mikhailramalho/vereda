int main()
{
  unsigned x = __VERIFIER_nondet_uint() % 10;
  __VERIFIER_assert(x < 10); // x in range
  return 0;
}
// Imported from ESBMC regression/neurosym/unknown
// Needs: --neurosym --neurosym-prog unknown-neurosym.sh --result-only
// RUN: --neurosym --neurosym-prog unknown-neurosym.sh --result-only
// MODE: FUTURE
// EXPECT: solver returned unknown
// EXPECT: SMT solver failed
// Vereda: verdict none
