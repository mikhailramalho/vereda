int main(void)
{
  unsigned i = 0;
  __ESBMC_loop_invariant(i <= 4 && i + 4294967295u == i - 1);
  while (i < 4)
    i++;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10853_unsigned_constant
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check --witness-output-yaml -
// RUN: --loop-invariant-check --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^  content:\n    \[\]$
