#include "header.h"

int main()
{
  f();
  assert(x==1);
}
// Imported from ESBMC regression/cbmc/01_cbmc_inline1
// RUN: --no-incremental-bmc
// FILES: module.c
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
