int main()
{
  unsigned x = __VERIFIER_nondet_uint() % 10;
  __VERIFIER_assert(x < 10); // x in range
  return 0;
}
// Imported from ESBMC regression/bitwuzllob/mono-formula-arg
// Needs: --bitwuzllob --bitwuzllob-prog z3 -smt2 %f
// RUN: --bitwuzllob --bitwuzllob-prog "z3 -smt2 %f"
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
