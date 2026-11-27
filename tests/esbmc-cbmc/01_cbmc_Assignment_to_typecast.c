int main()
{
  int i;

  i|=2;
  i|=2;

  void *p;
  
  p+=1;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Assignment_to_typecast
// EXPECT: ^VERIFICATION SUCCESSFUL$
