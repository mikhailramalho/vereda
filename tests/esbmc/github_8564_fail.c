#include <assert.h>
#include <stdio.h>

int __VERIFIER_nondet_int(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x >= 99 && x <= 999);
  int r = printf("%d", x);
  assert(r == 3);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8564_fail
// EXPECT: ^VERIFICATION FAILED$
