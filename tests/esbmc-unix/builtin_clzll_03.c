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
// Needs: --clz-zero-check
// RUN: --clz-zero-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
