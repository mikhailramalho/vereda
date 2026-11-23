int main()
{
  unsigned x = __VERIFIER_nondet_uint() % 10;
  __VERIFIER_assert(x < 10); // x in range
  return 0;
}
// Imported from ESBMC regression/bitwuzllob/mono-no-verdict
// Needs: --bitwuzllob --bitwuzllob-prog mute-mallob.sh --result-only
// RUN: --bitwuzllob --bitwuzllob-prog mute-mallob.sh --result-only
// MODE: FUTURE
// EXPECT: no sat/unsat verdict
// EXPECT: SMT solver failed
// Vereda: verdict none
