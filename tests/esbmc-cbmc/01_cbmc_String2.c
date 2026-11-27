char *p="abc";

int main()
{
  int input = -50;
  //char ch;

  /* should result in bounds violation */  
  //assert(input>=0 && input<3);
  p[input];
}
// Imported from ESBMC regression/cbmc/01_cbmc_String2
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
