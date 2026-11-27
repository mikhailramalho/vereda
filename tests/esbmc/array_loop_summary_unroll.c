// __ESBMC_unroll(2) stops the loop by a[2]; a summary would write a[3..7]
// out of bounds.
int main()
{
  int a[3] = {0};
  #pragma unroll 3
  for (unsigned j = 0; j < 8; j++)
    a[j] = (int)(j + 1);
  return 0;
}
// Imported from ESBMC regression/esbmc/array_loop_summary_unroll
// RUN: --no-unwinding-assertions --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
