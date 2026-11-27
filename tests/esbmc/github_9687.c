extern int __VERIFIER_nondet_int(void);
int main(void)
{
  int x = __VERIFIER_nondet_int();
  int y = x / 2;
  return y;
}
// Imported from ESBMC regression/esbmc/github_9687
// Needs: --witness-output-yaml
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --no-assertions --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
