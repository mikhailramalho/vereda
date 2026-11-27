int a[4];
int *p;

int main()
{
  int i;

  // normal
  a[1]=1;

  // not normal
  2[a]=2;
  
  assert(a[2]==2);
  
  p=a;

  // also a bit strange
  assert(2[p]==2);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Array_Access3
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
