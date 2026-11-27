int g;

void f()
{
  g=1;
}

int main() {
  assert(g==0);
  
  g==0?f():g;

  assert(g==1);
}
// Imported from ESBMC regression/cbmc/01_cbmc_void_ifthenelse
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
