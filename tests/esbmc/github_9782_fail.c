// #9782: the goto skips j's initializer, so j is indeterminate at J
#include <assert.h>
int __VERIFIER_nondet_int(void);
int main(void)
{
  if (__VERIFIER_nondet_int())
    goto J;
  unsigned char j = 3;
J:
  assert(j >= 3);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9782_fail
// EXPECT: ^VERIFICATION FAILED$
