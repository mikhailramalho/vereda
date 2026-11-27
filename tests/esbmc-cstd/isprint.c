#include <ctype.h>

int main ()
{
  int i = 0x20;

  while (i < 0x7E) 
    assert(isprint(i++));

  return 0;
}
// Imported from ESBMC regression/cstd/isprint
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
