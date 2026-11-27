#include <stdlib.h>
#include <assert.h>

int main()
{
  long long value = strtoll("100", NULL, 10);
  assert(value == 200); // deliberately wrong: value is 100, not 200
  return 0;
}
// Imported from ESBMC regression/esbmc/github_1016_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
