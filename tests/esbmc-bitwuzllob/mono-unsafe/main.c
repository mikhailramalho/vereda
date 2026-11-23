int main()
{
  unsigned x = __VERIFIER_nondet_uint() % 10;
  __VERIFIER_assert(x < 5); // x less than five
  return 0;
}
// Imported from ESBMC regression/bitwuzllob/mono-unsafe
// Needs: --bitwuzllob --bitwuzllob-prog fake-mallob.sh --bitwuzllob-model-prog z3 -in
// RUN: --bitwuzllob --bitwuzllob-prog fake-mallob.sh --bitwuzllob-model-prog "z3 -in"
// MODE: FUTURE
// EXPECT: ^\[Counterexample\]$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
