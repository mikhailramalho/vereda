extern int __VERIFIER_nondet_int(void);
int main(void)
{
  unsigned x = 0, i = 0;
  if (__VERIFIER_nondet_int())
  {
    for (int t = 0; t < 20; t++)
      x++;
  }
  __ESBMC_loop_invariant(i <= 3 && x == 0);
  while (i < 3)
    i++;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10853_depth
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check --depth 30 --witness-output-yaml
// RUN: --loop-invariant-check --depth 30 --witness-output-yaml - --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^  content:\n    \[\]$
