char *p;

int main()
{
  p="";

  // this is bad
  *p=1;
}
// Imported from ESBMC regression/cbmc/01_cbmc_String3
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
