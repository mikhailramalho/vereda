int main(void)
{
  unsigned i = 0, j = 0;
  __ESBMC_loop_invariant(i <= 10);
  while (i < 10)
    i++;
  i = 20;
  __ESBMC_loop_invariant(j <= 5 || i <= 10);
  while (j < 5)
    j++;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10853_or_invariant
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check --witness-output-yaml -
// RUN: --loop-invariant-check --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: (?s)line: 5\s+column: \d+\s+function: "main"\s+value: "i <= 10"
// EXPECT: (?s)line: 9\s+column: \d+\s+function: "main"\s+value: "j <= 5 \|\| i <= 10"
