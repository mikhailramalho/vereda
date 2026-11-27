#include <ctype.h>

int main ()
{
  int i = 0x10;

  while (i < 0x7E) 
    assert(isprint(i++));

  return 0;
}
// Imported from ESBMC regression/cstd/isprint_bug
// EXPECT: ^VERIFICATION FAILED$
