#include <assert.h>
int main(void)
{
  int n = 3;
  int a[2][n];
  n = 5;
  a[1][0] = 7;
  assert(*(&a[0][0] + 3) == 7);
  assert(sizeof(a) == 24);
  return 0;
}
// Imported from ESBMC regression/esbmc/vla_size_fixed_array
// EXPECT: ^VERIFICATION SUCCESSFUL$
