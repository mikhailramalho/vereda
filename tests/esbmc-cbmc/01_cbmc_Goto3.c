int main() {
  int a=0, b=0;
  int x;

  for(x=0; x<3; x++) {
    break;

    b=1;
  }
 
  assert(a==b);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Goto3
// EXPECT: ^VERIFICATION SUCCESSFUL$
