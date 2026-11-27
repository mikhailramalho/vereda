int main(void)
{
  unsigned k = 0;
  while (k < 5)
  {
    unsigned j = 0;
    __ESBMC_loop_invariant(j <= 1 && k < 2);
    for (; j < 1; j++)
      ;
    k++;
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10853_partial_unwind
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check --witness-output-yaml
// RUN: --loop-invariant-check --unwind 2 --no-unwinding-assertions --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^  content:\n    \[\]$
