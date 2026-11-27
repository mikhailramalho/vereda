#include <ctype.h>

int main ()
{
  int i=0;
  char str[]="abc4BC";
  while (str[i])
  {
    assert(isalpha(str[i]));
    i++;
  }
  return 0;
}
// Imported from ESBMC regression/cstd/isalpha_bug
// EXPECT: ^VERIFICATION FAILED$
