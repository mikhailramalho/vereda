int main() {
  double x;
  int y;
  
  x=2;  
  x-=0.6;
  y=x; // this yields 1.4, which is cut off
  
  assert(y==1);

  x=2;  
  x-=0.4;
  y=x; // this yields 1.6, which is cut off, too!
  
  assert(y==1);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Fixedbv1
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
