#include <assert.h>
#include <stdio.h>

unsigned long long __VERIFIER_nondet_ull(void);

int main(void)
{
  unsigned long long x = __VERIFIER_nondet_ull();
  // %u prints x converted to unsigned int; no value of x is ruled out.
  int r = printf("%u", x);
  assert(x < 10000000000ULL);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8564_width_fail
// EXPECT: ^VERIFICATION FAILED$
