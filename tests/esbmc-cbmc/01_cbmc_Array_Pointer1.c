int a[4];
int *p;

int main() {
  int j;

  a[1] = 1;

  p = a;
  p++;
  *p = 1;

  j = a[1];

  assert(j == 1);
  
  // assignment with cast
  p=(int *)a;
  assert(*p==0);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Array_Pointer1
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
