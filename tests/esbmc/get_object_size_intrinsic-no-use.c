#include <stdlib.h>

int main()
{
  char *a = malloc(200);
  __ESBMC_get_object_size(a);
}
// Imported from ESBMC regression/esbmc/get_object_size_intrinsic-no-use
// Needs: __ESBMC_get_object_size
// RUN: --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
