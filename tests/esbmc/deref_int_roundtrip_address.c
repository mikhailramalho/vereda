#include <assert.h>
#include <stdint.h>

int main()
{
  int x = 7;
  int *p = (int *)(uintptr_t)&x;
  assert(*p == 7);
  return 0;
}
// Imported from ESBMC regression/esbmc/deref_int_roundtrip_address
// EXPECT: ^VERIFICATION SUCCESSFUL$
