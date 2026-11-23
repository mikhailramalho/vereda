int main()
{
  unsigned x = __VERIFIER_nondet_uint() % 10;
  __VERIFIER_assert(x < 10); // x in range
  return 0;
}
// Imported from ESBMC regression/bitwuzllob/mono-incompatible-strategy
// Needs: --bitwuzllob --k-induction --bitwuzllob-prog fake-mallob.sh
// RUN: --bitwuzllob --k-induction --bitwuzllob-prog fake-mallob.sh
// MODE: FUTURE
// EXPECT: does not support --k-induction
// Vereda: verdict none
