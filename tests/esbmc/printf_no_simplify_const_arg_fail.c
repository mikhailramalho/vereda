#include <assert.h>
#include <stdio.h>

int main()
{
  /* Non-vacuity control: the return value is pinned to the exact length, so
     asserting the wrong length must fail rather than pass on a widened range. */
  int a = printf("%02X", (unsigned)0xAB);
  assert(a == 3);
}
// Imported from ESBMC regression/esbmc/printf_no_simplify_const_arg_fail
// EXPECT: ^VERIFICATION FAILED$
