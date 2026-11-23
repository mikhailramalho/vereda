#include <stdlib.h>
#include <string.h>
#include <assert.h>

int main(void)
{
  char *result = getenv(NULL);
  return 0;
}
// Imported from ESBMC regression/esbmc/getenv1
// RUN: --unwind 1
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
