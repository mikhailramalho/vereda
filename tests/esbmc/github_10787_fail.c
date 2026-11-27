// &b.a[2] points past the one-past-the-end address of b.a (C11 6.5.6p8).
struct
{
  int a[1];
  int end;
} b;
int main()
{
  int c = 2;
  int *p = &b.a[c];
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10787_fail
// RUN: --no-incremental-bmc
// EXPECT: ^  FAILED +\[main\.pointer-arithmetic-out-of-bounds\.\d+\] +line 10 +pointer arithmetic out of bounds: array upper bound$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  FAILED +\[main\.pointer-arithmetic-out-of-bounds\.\d+\] +line 10 +pointer arithmetic out of bounds: array upper bound$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
