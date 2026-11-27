// #7907: a vector written and read back through a pointer to a
// nondeterministic element of an array of vectors.
#include <assert.h>

typedef int v4i __attribute__((__vector_size__(16)));

int __VERIFIER_nondet_int(void);

int main(void)
{
  v4i a[3];
  int k = __VERIFIER_nondet_int();
  __VERIFIER_assume(k >= 0 && k < 3);
  v4i *q = a + k;
  *q = (v4i){1, 2, 3, 4};
  v4i c = *q;
  assert(c[3] == 4);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7907_simd
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
