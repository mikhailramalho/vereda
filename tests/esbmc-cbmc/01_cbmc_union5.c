#include <stdio.h>
#include <assert.h>

union u_type
{
  int i;
  char ch;
};

int main() {
  union u_type u;
  
  u.i=1;
  assert(u.i==1);
  
  u.ch=2;
  assert(u.ch==2);
  printf("u.i = %d", u.i);
  assert(u.i==2);
}
// Imported from ESBMC regression/cbmc/01_cbmc_union5
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
