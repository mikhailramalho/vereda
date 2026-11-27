#include <string.h>

int main()
{
  char a;
  int b;
  memcpy(&a, &b, sizeof(char) + 1);
  assert(a == b);
  return 0;  
}
// Imported from ESBMC regression/cstd/intrinsic_memcpy_bug3
// EXPECT: ^VERIFICATION FAILED$
