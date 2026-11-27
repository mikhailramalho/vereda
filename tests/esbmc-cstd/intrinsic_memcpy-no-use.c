#include <string.h>
#include <assert.h>

#define T unsigned long long

int main()
{
  // basic memcpy
  T a0, a1;
  a1 = 0;
  memcpy(&a0, &a1, sizeof(T));
  assert(a0 == a1);

  return 0;
}
// Imported from ESBMC regression/cstd/intrinsic_memcpy-no-use
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
