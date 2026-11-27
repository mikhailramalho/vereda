float __VERIFIER_nondet_float();

int main()
{
  float x = __VERIFIER_nondet_float();
  assert(x==x);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_536_boolector
// Needs: --smt-model
// RUN: --smt-model
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
