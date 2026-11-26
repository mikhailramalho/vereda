int main()
{
  unsigned x = __VERIFIER_nondet_uint() % 10;
  __VERIFIER_assert(x < 10); // x in range
  return 0;
}
// Imported from ESBMC regression/bitwuzllob/mono-formula-too
// Needs: --bitwuzllob --bitwuzllob-prog fake-mallob.sh
// RUN: --bitwuzllob --bitwuzllob-prog fake-mallob.sh --smt-formula-too
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
