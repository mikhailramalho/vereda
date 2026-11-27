int main(void)
{
  int a[2][2][2];
  int s = 0;

  a[1][1][1] = 4;

  for (int i = 0; i < a[1][1][1]; i++)
    s++;

  __VERIFIER_assert(s == 5); // the same bound, asserted wrong
  return 0;
}
// Imported from ESBMC regression/esbmc/nested_array_3d_loop_bound_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
