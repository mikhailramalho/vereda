int main()
{
  int x[1][1][3];
  x[0][0][0] = 80;
  x[0][0][2] = 0;
  __VERIFIER_assume((x[0][0][0] >= 77) && (x[0][0][0] <= 87));
  __VERIFIER_assume((x[0][0][2] >= -64) && (x[0][0][2] <= 64));
  __VERIFIER_assert(1);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_661_1_true
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
