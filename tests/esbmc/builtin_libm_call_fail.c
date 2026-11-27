#include <assert.h>

int main(void)
{
  int e = 0;
  __builtin_frexp(8.0, &e);
  assert(e == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/builtin_libm_call_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
