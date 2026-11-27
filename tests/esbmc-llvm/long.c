#include <assert.h>
#include <stdio.h>

int main()
{
  long x = 3;
  return 0;
}
// Imported from ESBMC regression/llvm/long
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
