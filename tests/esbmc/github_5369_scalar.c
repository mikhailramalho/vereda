/* Two reads of one location through a pointer the value-set cannot resolve,
 * with no write between them, must agree. make_failed_symbol() mints a fresh
 * free value per read, so they are free to differ (#5369). --no-pointer-check
 * keeps the unresolvable dereference itself from being reported, which would
 * mask the assertion. */
#include <assert.h>

unsigned long __VERIFIER_nondet_ulong(void);

int main(void)
{
  int *p = (int *)__VERIFIER_nondet_ulong();
  int a = *p;
  int b = *p;
  assert(a == b);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_5369_scalar
// EXPECT: ^VERIFICATION SUCCESSFUL$
