// #8006
int main()
{
  int y = 0;
  int *p = &y;
  int a = 0;
  {
    int x = 1;
    p = &x;
    a = *p + 1;
  }
  int b = *p + 1; // x is dead
  return a + b;
}
// Imported from ESBMC regression/esbmc/github_8006_scope
// RUN: --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict SUCCESSFUL
