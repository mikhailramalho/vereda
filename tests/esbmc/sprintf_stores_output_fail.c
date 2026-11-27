#include <stdio.h>

int main(void)
{
  char b[4];
  sprintf(b, "%s", "hello");
  return 0;
}
// Imported from ESBMC regression/esbmc/sprintf_stores_output_fail
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\s*FAILED\s+\[main\.array-bounds-violated\.1\]\s+line 6\s+dereference failure: array bounds violated$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
