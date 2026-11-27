int main()
{
  int var;
  unsigned long int ptr = &var;
  int *c = (int *)ptr;
  *c = 42;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_382_4
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
