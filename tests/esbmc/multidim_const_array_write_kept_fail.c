// Anti-vacuity twin: the surviving write must carry the value it wrote.
#include <assert.h>

int main(void)
{
  int r[2][2];
  _Bool c = __VERIFIER_nondet_bool();

  __VERIFIER_assume(c);
  if (c)
  {
    r[0][0] = 1;
    r[0][1] = 2;
  }

  assert(r[0][0] == 2);
  return 0;
}
// Imported from ESBMC regression/esbmc/multidim_const_array_write_kept_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
