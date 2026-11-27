int bar(int, int);
int bar(int x, int y)
{
  return x+y;
}

int main()
{
  int x = bar(1,2); 
}
// Imported from ESBMC regression/llvm/funcs_redef
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
