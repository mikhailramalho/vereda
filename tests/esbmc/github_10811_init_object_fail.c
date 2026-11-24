#include <assert.h>
#include <stdio.h>

/* #10811: only the object p points to may be written. */
_Bool __VERIFIER_nondet_bool(void);
int a[2] = {1, 2};
int b[2];

int main()
{
  int *p = __VERIFIER_nondet_bool() ? a : b;
  __VERIFIER_assume(p == a);
  __ESBMC_init_object(p);
  assert(a[0] == 1);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10811_init_object_fail
// Needs: __ESBMC_init_object
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
