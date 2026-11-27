int main()
{
  char a;
  char b = a = 'a';
  
  assert(a=='a');
  assert(b=='a');
}

// Imported from ESBMC regression/cbmc/01_cbmc_Initialization1
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
