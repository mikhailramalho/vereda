int main()
{
  unsigned x = __VERIFIER_nondet_uint() % 10;
  __VERIFIER_assert(x < 10); // x in range
  return 0;
}
// Imported from ESBMC regression/bitwuzllob/mono-diverging-model
// Needs: --bitwuzllob --bitwuzllob-prog always-sat-mallob.sh --bitwuzllob-model-prog z3 -in
// RUN: --bitwuzllob --bitwuzllob-prog always-sat-mallob.sh --bitwuzllob-model-prog "z3 -in"
// MODE: FUTURE
// EXPECT: refusing to build a counterexample from a diverging model
// Vereda: verdict none
