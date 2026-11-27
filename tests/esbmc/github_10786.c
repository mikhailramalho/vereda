// #10786: the VLA size `0 & 0 / 0` is evaluated, so its division is checked.
int main()
{
  int z = 0;
  sizeof(int[3][0 & 1 / z]);
  sizeof(int[0 & 0 / 0]);
}
// Imported from ESBMC regression/esbmc/github_10786
// RUN: --std c11 --multi-property --no-incremental-bmc
// MODE: FUTURE
// EXPECT: \[main\.division-by-zero\.1\]  line 5  division by zero$
// EXPECT: \[main\.division-by-zero\.2\]  line 6  division by zero$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict FAILED
