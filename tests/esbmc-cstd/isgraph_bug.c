#include <ctype.h>

int main ()
{
  int i = 0x10;

  while (i < 0x7F) 
    assert(isgraph(i++));

  return 0;
}
// Imported from ESBMC regression/cstd/isgraph_bug
// EXPECT: ^VERIFICATION FAILED$
