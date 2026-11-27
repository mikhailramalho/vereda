#include <string.h>

int main ()
{
  char str[] = "fcba73";
  char keys[] = "1234567890";
  int i;
  i = strcspn (str,keys);
  assert(i == 5);
  return 0;
}
// Imported from ESBMC regression/cstd/strcspn_bug
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
