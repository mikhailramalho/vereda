struct Base
{
  int ss[128];
};

int main()
{
  struct Base x, *y = &x;
  y->ss;
}
// Imported from ESBMC regression/esbmc/github_1934-1
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
