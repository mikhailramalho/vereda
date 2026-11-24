#include <string.h>

int main ()
{
  char str[] ="This is a simple string";
  char * pch;
  pch = strstr (str,"simple");
  strncpy (pch,"sample",5);
  assert(!strcmp(str, "This is a sample string"));
  return 0;
}
// Imported from ESBMC regression/cstd/strstr_bug
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// Vereda: ESBMC marks this KNOWNBUG; the program is correct (strncpy writes "sampl" over "simpl")
// MODE: KNOWNBUG
// Vereda: verdict SUCCESSFUL
