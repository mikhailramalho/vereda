// &s[i].f names a member of s[i], so s[i] must exist: i == 2 is out of range
// even though &s[2] alone would be valid.
struct pair
{
  int f, g;
} s[2];
int main()
{
  int i = 2;
  int *p = &s[i].f;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10787-member_fail
// RUN: --no-incremental-bmc
// EXPECT: ^  FAILED +\[main\.pointer-arithmetic-out-of-bounds\.\d+\] +line 10 +pointer arithmetic out of bounds: array `s' upper bound$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  FAILED +\[main\.pointer-arithmetic-out-of-bounds\.\d+\] +line 10 +pointer arithmetic out of bounds: array `s' upper bound$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
