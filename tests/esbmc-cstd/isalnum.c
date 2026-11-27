#include <ctype.h>

int main ()
{
  int i = 0;
  char str[]="c3po...";

  while (isalnum(str[i])) i++;

  assert(i == 4);
  return 0;
}
// Imported from ESBMC regression/cstd/isalnum
// EXPECT: ^VERIFICATION SUCCESSFUL$
