#include <stdio.h>
#include <ctype.h>

int main ()
{
  int i=0;
  while (i < 32)
  {
    assert(iscntrl(i));
    i++;
  }
  return 0;
}
// Imported from ESBMC regression/cstd/iscntrl
// EXPECT: ^VERIFICATION SUCCESSFUL$
