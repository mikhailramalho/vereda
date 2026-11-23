int main()
{
  unsigned x = __VERIFIER_nondet_uint() % 10;
  __VERIFIER_assert(x < 10); // x in range
  return 0;
}
// Imported from ESBMC regression/bitwuzllob/mono-model-cutoff
// Needs: --bitwuzllob --bitwuzllob-prog always-sat-mallob.sh --bitwuzllob-model-prog cutoff-model.sh
// RUN: --bitwuzllob --bitwuzllob-prog always-sat-mallob.sh --bitwuzllob-model-prog cutoff-model.sh
// MODE: FUTURE
// EXPECT: ^ERROR: SMT solver process failed
// Vereda: verdict none
