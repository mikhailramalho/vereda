#include <string.h>
#include <assert.h>

int main ()
{
  char str[] ="- This, a sample string.";
  char * pch;

  pch = strtok (str," ,.-");
  assert(!strcmp(pch, "This"));

  pch = strtok (NULL, " ,.-");
  assert(!strcmp(pch, "a"));

  pch = strtok (NULL, " ,.-");
  assert(!strcmp(pch, "sample"));

  pch = strtok (NULL, " ,.-");
  assert(!strcmp(pch, "string"));

  return 0;
}
// Imported from ESBMC regression/cstd/strtok
// RUN: --unwind 7
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
