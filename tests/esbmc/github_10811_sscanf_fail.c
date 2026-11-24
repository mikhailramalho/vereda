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
  sscanf("5", "%d", p);
  assert(a[0] == 1);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10811_sscanf_fail
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: FUTURE test passed unexpectedly; promote it to CORE
// MODE: CORE
