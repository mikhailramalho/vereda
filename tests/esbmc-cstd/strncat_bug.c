#include <string.h>

int main ()
{
  char str1[20];
  char str2[20];
  strcpy (str1,"To be ");
  strcpy (str2,"or not to be");
  strncat (str1, str2, 6);

  assert(strcmp("To be or not", str1));

  return 0;
}
// Imported from ESBMC regression/cstd/strncat_bug
// RUN: --unwind 13
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: FUTURE test passed unexpectedly; promote it to CORE
// MODE: CORE
