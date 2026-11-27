int main() {
  int x=-4,y;
  // should succeed
  y=x>>1;
  x>>=1;
  assert(x==-2);
  assert(y==-2);
  
  // should also work with mixed types
  assert(((-2)>>1u)==-1);
}
// Imported from ESBMC regression/cbmc/01_cbmc_ASHR
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
