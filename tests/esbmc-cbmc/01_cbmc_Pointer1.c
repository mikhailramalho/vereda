int main()
 {
  int a, b, c, *p; 
 
  if(c) p=&a; else p=&b;
  
  *p=3;
  
  assert(b==3 || a==3);
 }
// Imported from ESBMC regression/cbmc/01_cbmc_Pointer1
// EXPECT: ^VERIFICATION SUCCESSFUL$
