#include <assert.h>
int g[4];
int main(void)
{
  int *p = &g[0];
  unsigned char k = __VERIFIER_nondet_uchar();
  __VERIFIER_assume(k < 2);
  ((unsigned char *)&p)[k] = ((unsigned char *)&p)[k];
  assert(p == &g[0]);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7855-byte-update-sds
// RUN: -m64
// EXPECT: ^VERIFICATION SUCCESSFUL$
