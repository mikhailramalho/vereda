int main()
{
  unsigned x = __VERIFIER_nondet_uint() % 10;
  __VERIFIER_assert(x < 10); // x in range
  return 0;
}
// Imported from ESBMC regression/bitwuzllob/mono-output-stdout
// Needs: --bitwuzllob --output - --bitwuzllob-prog fake-mallob.sh
// RUN: --bitwuzllob --output - --bitwuzllob-prog fake-mallob.sh
// MODE: FUTURE
// EXPECT: ignoring --output -
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
