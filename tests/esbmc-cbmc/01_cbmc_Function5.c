#include <stdio.h>

int main() {
  int *p;
  int a=3;

  p=&a;
  printf("Value of integer pointed by p: %d\n",*p);
  p++;
  printf("Next value of integer pointed by p: %d\n",*p);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Function5
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
