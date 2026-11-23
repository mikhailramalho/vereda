int main()
{
  unsigned x = __VERIFIER_nondet_uint() % 10;
  __VERIFIER_assert(x < 10); // x in range
  return 0;
}
// Imported from ESBMC regression/bitwuzllob/mono-signal-death
// Needs: --bitwuzllob --bitwuzllob-prog crash-mallob.sh --result-only
// RUN: --bitwuzllob --bitwuzllob-prog crash-mallob.sh --result-only
// MODE: FUTURE
// EXPECT: died with signal .*discarding its verdict
// EXPECT: SMT solver failed
// Vereda: verdict none
