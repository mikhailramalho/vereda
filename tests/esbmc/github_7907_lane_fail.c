// #7907: the lane written through the pointer is the one it names.
#include <assert.h>

typedef int v4i __attribute__((__vector_size__(16)));

int __VERIFIER_nondet_int(void);

int main(void)
{
  v4i s = {1, 2, 3, 4};
  v4i *p = &s;
  int k = __VERIFIER_nondet_int();
  __VERIFIER_assume(k >= 0 && k < 4);
  (*p)[k] = 9;
  assert(s[1] == 2);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7907_lane_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
