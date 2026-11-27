#include <string.h>

#define T char

int main()
{
  T a, b;
  memcpy(&a, &b, sizeof(T) + 1);
  assert(a == b);
  return 0;  
}
// Imported from ESBMC regression/cstd/intrinsic_memcpy_bug1
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
