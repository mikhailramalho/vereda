#include <ctype.h>

int main ()
{
  int i = 0;
  char str[]="c3po...";

  while (isalnum(str[i])) i++;

  assert(i == 5);
  return 0;
}
// Imported from ESBMC regression/cstd/isalnum_bug
// EXPECT: ^VERIFICATION FAILED$
