#include <ctype.h>

int main ()
{
  int i = 0x41;

  while (i < 0x7A) 
    assert(islower(i++));

  return 0;
}
// Imported from ESBMC regression/cstd/islower_bug
// EXPECT: ^VERIFICATION FAILED$
