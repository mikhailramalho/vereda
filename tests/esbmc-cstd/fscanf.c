#include <stdio.h>
#include <assert.h>
int main(int argc, char *argv[])
{
  int m, n = 42;
  fscanf(
    stdin,
    "%10d",
    &m);
  assert(n == 42);
}
// Imported from ESBMC regression/cstd/fscanf
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check -m64 --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
