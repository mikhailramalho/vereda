int main(void)
{
  unsigned i = 0;
  __ESBMC_loop_invariant(i <= 5);
  while (i < 5)
    i++;
  i = 100;
  __ESBMC_loop_invariant(i <= 5);
  do
  {
    i = 0;
  } while (i > 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10853_do_while_base_dropped
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check --claim 1 --claim 2 --claim 4 --witness-output-yaml
// RUN: --loop-invariant-check --claim 1 --claim 2 --claim 4 --witness-output-yaml - --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: (?s)line: 5\s+column: \d+\s+function: "main"\s+value: "i <= 5"
// EXPECT: (?s)\A(?!.*line: 9$)
