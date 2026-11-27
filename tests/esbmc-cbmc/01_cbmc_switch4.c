int main(){

  int x;

  switch(x){
  case 0:
    goto end;
  default:
    x = 0;
  }
 end:
  assert(x==0);
}
// Imported from ESBMC regression/cbmc/01_cbmc_switch4
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
