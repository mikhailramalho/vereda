#include <stdio.h>
const int c = 3;
int main()
{
  sscanf("5", "%d", (int *)&c);
  return 0;
}
// Imported from ESBMC regression/esbmc/const_object_write_sscanf_fail
// RUN: --no-incremental-bmc
// EXPECT: ^  dereference failure: write access to const object `c'$
// EXPECT: ^VERIFICATION FAILED$
