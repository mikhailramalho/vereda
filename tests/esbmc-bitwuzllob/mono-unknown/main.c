int main()
{
  unsigned x = __VERIFIER_nondet_uint() % 10;
  __VERIFIER_assert(x < 10); // x in range
  return 0;
}
// Imported from ESBMC regression/bitwuzllob/mono-unknown
// Needs: --bitwuzllob --bitwuzllob-prog unknown-mallob.sh --result-only
// RUN: --bitwuzllob --bitwuzllob-prog unknown-mallob.sh --result-only
// MODE: FUTURE
// EXPECT: solver returned unknown
// EXPECT: SMT solver failed
// Vereda: verdict none
