int g;

void count(void)
{
  unsigned i = 0;
  __ESBMC_loop_invariant(i <= 3 && g == 0);
  for (; i < 3; i++)
    ;
}

int main(void)
{
  g = 1;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10853_function
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check --witness-output-yaml
// RUN: --loop-invariant-check --function count --witness-output-yaml - --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^  content:\n    \[\]$
