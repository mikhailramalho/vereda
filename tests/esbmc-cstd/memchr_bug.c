#include <string.h>

int main ()
{
  char * pch;
  char str[] = "Example string";
  pch = (char*) memchr (str, 'p', strlen(str));

  assert((pch - str + 1) == 4);
  return 0;
}
// Imported from ESBMC regression/cstd/memchr_bug
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
