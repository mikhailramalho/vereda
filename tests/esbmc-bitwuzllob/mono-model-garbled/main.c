int main()
{
  unsigned x = __VERIFIER_nondet_uint() % 10;
  __VERIFIER_assert(x < 10); // x in range
  return 0;
}
// Imported from ESBMC regression/bitwuzllob/mono-model-garbled
// Needs: --bitwuzllob --bitwuzllob-prog always-sat-mallob.sh --bitwuzllob-model-prog garbage-model.sh
// RUN: --bitwuzllob --bitwuzllob-prog always-sat-mallob.sh --bitwuzllob-model-prog garbage-model.sh
// MODE: FUTURE
// EXPECT: the local model solver is unavailable; cannot build a counterexample
// EXPECT: SMT solver failed
// Vereda: verdict none
