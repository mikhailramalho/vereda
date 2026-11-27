#include <assert.h>
int main(void)
{
  int x = __VERIFIER_nondet_int();
  if (x > 0)
    assert(x > 0);
  assert(x != 42);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_6422_6
// RUN: --multi-property
// EXPECT: ^VERIFICATION FAILED$
