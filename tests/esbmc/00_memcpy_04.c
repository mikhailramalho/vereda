#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define N 1
int main() {
  double *dev_a = (double*)malloc(sizeof(double));
  double a = 1;
  memcpy(dev_a,&a,sizeof(double));
  assert(dev_a[0]==2);
  return 0;
}

// Imported from ESBMC regression/esbmc/00_memcpy_04
// RUN: --force-malloc-success --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
