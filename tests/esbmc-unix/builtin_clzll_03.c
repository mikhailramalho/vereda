#include <stdint.h>
#include <assert.h>

int main()
{
  uint64_t x;
  int clz;

  x = 0;
  __builtin_clzll(x);

  return 0;
}
// Imported from ESBMC regression/esbmc-unix/builtin_clzll_03
// RUN: --ub-count-zero-check --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
