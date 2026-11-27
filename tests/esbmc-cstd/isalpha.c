#include <ctype.h>

int main ()
{
  int i=0;
  char str[]="abcABC";
  while (str[i])
  {
    assert(isalpha(str[i]));
    i++;
  }
  return 0;
}
// Imported from ESBMC regression/cstd/isalpha
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
