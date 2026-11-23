#include <stdint.h>
#include <assert.h>
int main()
{
  int32_t x = 2;
  x <<= 31;
  x <<= 31;
  assert(x == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_578_fail4
// RUN: --overflow-check
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
