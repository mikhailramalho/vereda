int g;
int main(void)
{
  __ESBMC_loop_invariant(g == 0);
  int g = 5;
  while (g < 10)
    g++;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10853_shadowed
// Needs: --loop-invariant-check --witness-output-yaml -
// RUN: --loop-invariant-check --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^  content:\n    \[\]$
// Vereda: verdict none
