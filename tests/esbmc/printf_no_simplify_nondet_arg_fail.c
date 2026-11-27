#include <assert.h>
#include <stdio.h>

unsigned __VERIFIER_nondet_uint(void);

int main()
{
  /* Over-pinning control: %02X of a genuinely unknown value spans 2..8
     characters, so the return value must stay a range, not fold to a
     constant. */
  unsigned v = __VERIFIER_nondet_uint();
  int r = printf("%02X", v);
  assert(r == 2);
}
// Imported from ESBMC regression/esbmc/printf_no_simplify_nondet_arg_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
