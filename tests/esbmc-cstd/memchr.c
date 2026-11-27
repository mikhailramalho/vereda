#include <string.h>

int main ()
{
  char * pch;
  char str[] = "Example string";
  pch = (char*) memchr (str, 'p', strlen(str));

  assert((pch - str + 1) == 5);
  return 0;
}
// Imported from ESBMC regression/cstd/memchr
// EXPECT: ^VERIFICATION SUCCESSFUL$
