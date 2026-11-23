int main()
{
  unsigned x = __VERIFIER_nondet_uint() % 10;
  __VERIFIER_assert(x < 5); // x less than five
  return 0;
}
// Imported from ESBMC regression/bitwuzllob/mono-result-only-model
// Needs: --bitwuzllob --bitwuzllob-prog fake-mallob.sh --bitwuzllob-model-prog z3 -in --result-only
// RUN: --bitwuzllob --bitwuzllob-prog fake-mallob.sh --bitwuzllob-model-prog "z3 -in" --result-only
// MODE: FUTURE
// EXPECT: ignoring --bitwuzllob-model-prog
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
