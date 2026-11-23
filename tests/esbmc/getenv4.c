#include <stdlib.h>
#include <string.h>
#include <assert.h>

int main(void)
{
  char *result = getenv("NONEXISTENT_VAR_12345");
  // This should fail if getenv returns NULL
  size_t len = strlen(result);
  assert(len >= 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/getenv4
// RUN: --unwind 22
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
