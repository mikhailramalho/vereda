#include <assert.h>

unsigned __VERIFIER_nondet_uint();

// `(a + b) * c` recomputes `a + b` after `a` changed, but only its symbol is
// assigned, so the symbol for `a + b` must not be reused (#7992).
int main()
{
  unsigned a = __VERIFIER_nondet_uint(), b = __VERIFIER_nondet_uint(), c = __VERIFIER_nondet_uint();
  unsigned x = a + b;
  a = a + 1u;
  unsigned y = (a + b) * c;
  unsigned z = a + b;
  unsigned w = (a + b) * c;
  assert(z == x);
  return (int)(y + w);
}
// Imported from ESBMC regression/esbmc/gcse_subexpr_stale_fail
// EXPECT: ^VERIFICATION FAILED$
