void exit(int status);

int main() {
  int x;
  
  if(x==10)
    exit(1);

  assert(x!=10);
}
// Imported from ESBMC regression/cbmc/01_cbmc_exit1
// EXPECT: ^VERIFICATION SUCCESSFUL$
