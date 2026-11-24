#include <assert.h>
int x = 0;
void *p;
int main()
{
  p = __VERIFIER_nondet_bool() ? (void *)&x : (void *)&p;
  __VERIFIER_assume(p == (void *)&p);
  __ESBMC_init_object(p);
  assert(x == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10811_init_object_self
// EXPECT: ^VERIFICATION SUCCESSFUL$
