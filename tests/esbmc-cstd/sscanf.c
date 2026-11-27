#include <stdio.h>
#include <assert.h>
int main(int argc, char *argv[])
{
  int life = 24;
  sscanf(argv[1],"%d", &life); 
  assert(life == 42);
  return 0;
}
// Imported from ESBMC regression/cstd/sscanf
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
