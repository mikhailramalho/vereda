#include <assert.h>

// Reusing the stale `(long)b + 1` made y equal x (#7992).
int main()
{
  _Bool b = 1;
  long x = (long)b + 1;
  b ^= 1;
  long y = (long)b + 1;
  assert(y == x);
}
// Imported from ESBMC regression/esbmc/gcse_typecast_lvalue_kill_fail
// Needs: --gcse
// RUN: --gcse
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
