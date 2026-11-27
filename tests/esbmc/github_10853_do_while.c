int main(void)
{
  unsigned i = 0;
  __ESBMC_loop_invariant(i <= 10);
  do
  {
    i++;
  } while (i < 10);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10853_do_while
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check --witness-output-yaml
// RUN: --loop-invariant-check --witness-output-yaml - --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: (?s)type: "loop_invariant"\s+location:\s+file_name: "[^"]*github_10853_do_while\.c"\s+line: 5\s+column: 3\s+function: "main"\s+value: "i <= 10"
