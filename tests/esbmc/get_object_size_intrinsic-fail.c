#include <stdlib.h>
#include <assert.h>

int main()
{
  char *a = malloc(200);
  assert(__ESBMC_get_object_size(a) == 2000); // Should fail, 2000 != 200
}
// Imported from ESBMC regression/esbmc/get_object_size_intrinsic-fail
// Needs: __ESBMC_get_object_size
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
