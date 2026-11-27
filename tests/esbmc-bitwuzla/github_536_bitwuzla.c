float __VERIFIER_nondet_float();

int main()
{
  float x = __VERIFIER_nondet_float();
  assert(x==x);
  return 0;
}
// Imported from ESBMC regression/bitwuzla/github_536_bitwuzla
// Needs: --smt-model
// RUN: --smt-model --bitwuzla
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
