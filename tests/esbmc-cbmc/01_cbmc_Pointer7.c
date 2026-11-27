int main()
{
  unsigned char u=255;
  signed char *p;
  
  p=(signed char *)&u;
  
  assert(*p==-1);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Pointer7
// EXPECT: ^VERIFICATION SUCCESSFUL$
