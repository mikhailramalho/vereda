int main()
{
  unsigned x = __VERIFIER_nondet_uint() % 10;
  __VERIFIER_assert(x < 5); // x less than five
  return 0;
}
// Imported from ESBMC regression/bitwuzllob/mono-result-only
// Needs: --bitwuzllob --bitwuzllob-prog fake-mallob.sh --result-only
// RUN: --bitwuzllob --bitwuzllob-prog fake-mallob.sh --result-only
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
