// A byte walk over one row of an array of byte arrays.
#include <assert.h>
unsigned char pool[4][8];
int main(void)
{
  int n = 0;
  for (unsigned char *q = pool[0]; q != pool[1]; ++q)
    n++;
  assert(n != 8);
  return 0;
}
// Imported from ESBMC regression/esbmc/byte_walk_row_fail
// EXPECT: ^VERIFICATION FAILED$
