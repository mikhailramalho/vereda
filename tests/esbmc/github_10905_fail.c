#include <assert.h>
#include <stdint.h>
unsigned __VERIFIER_nondet_uint(void);
int main(void)
{
  uint32_t buf[1];
  unsigned off = __VERIFIER_nondet_uint();
  __VERIFIER_assume(off < 4);
  uint32_t *p1 = buf;
  __VERIFIER_assume((uintptr_t)p1 != 4);
  uint32_t *p2 = (uint32_t *)(uintptr_t)4 + off;
  if ((intptr_t)p1 == (intptr_t)p2)
    assert(p1 != p2);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10905_fail
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
