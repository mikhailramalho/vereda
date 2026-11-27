// An unused pointer difference kept by --no-slice is encoded as a ptrdiff_t.
#include <assert.h>
#include <stddef.h>
unsigned __VERIFIER_nondet_uint(void);
int main(void)
{
  long a[2];
  ptrdiff_t n = (a + 1) - a;
  (void)n;
  unsigned x = __VERIFIER_nondet_uint();
  assert(x * 2 != 1);
  return 0;
}
// Imported from ESBMC regression/esbmc/pointer_diff_unused
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
