#include <string.h>

int main ()
{
  char str[] = "almost every programmer should know memset!";
  memset (str,'-',6);

  int i = 0;
  while(i < 6) assert(str[i++] == '-');

  return 0;
}
// Imported from ESBMC regression/cstd/memset
// EXPECT: ^VERIFICATION SUCCESSFUL$
