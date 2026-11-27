int __VERIFIER_nondet_int(void);

int main(void)
{
  int a[2][2][2][2];
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 2);

  for (int x = 0; x < 2; x++)
    for (int y = 0; y < 2; y++)
      for (int z = 0; z < 2; z++)
        for (int w = 0; w < 2; w++)
          a[x][y][z][w] = 1;

  a[1][1][1][1] = 4;

  __VERIFIER_assert(a[i][1][1][1] >= 4); // the same read, asserted wrong
  return 0;
}
// Imported from ESBMC regression/esbmc/nested_array_4d_outer_read_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
