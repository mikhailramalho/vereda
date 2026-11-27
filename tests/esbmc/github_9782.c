// #9782: only the path that runs j's initializer may rely on its value
#include <assert.h>
int __VERIFIER_nondet_int(void);
int main(void)
{
  int took = __VERIFIER_nondet_int();
  if (took)
    goto J;
  unsigned char j = 3;
J:
  if (!took)
    assert(j == 3);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9782
// EXPECT: ^VERIFICATION SUCCESSFUL$
