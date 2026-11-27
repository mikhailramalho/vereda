// #7907: one lane of a vector written through a pointer at a nondeterministic
// index.
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
  assert(s[k] == 9);
  assert(k == 1 || s[1] == 2);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7907_lane
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
