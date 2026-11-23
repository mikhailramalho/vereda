int main()
{
  unsigned x = __VERIFIER_nondet_uint() % 10;
  __VERIFIER_assert(x < 5); // x less than five
  return 0;
}
// Imported from ESBMC regression/neurosym/basic-unsafe
// Needs: --neurosym --neurosym-prog fake-neurosym.sh --neurosym-model-prog z3 -in
// RUN: --neurosym --neurosym-prog fake-neurosym.sh --neurosym-model-prog "z3 -in"
// MODE: FUTURE
// EXPECT: ^\[Counterexample\]$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
