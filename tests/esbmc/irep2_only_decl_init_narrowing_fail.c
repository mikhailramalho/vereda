/* The initialiser is truncated to the declared width, so it is not the
   int-width sum whenever that sum does not fit. */
#include <assert.h>

int __VERIFIER_nondet_int(void);

int main(void)
{
  _ExtInt(10) x = __VERIFIER_nondet_int();
  _ExtInt(10) y = __VERIFIER_nondet_int();
  _ExtInt(10) z = x + y;
  assert(z == (int)x + (int)y);
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_decl_init_narrowing_fail
// EXPECT: ^VERIFICATION FAILED$
