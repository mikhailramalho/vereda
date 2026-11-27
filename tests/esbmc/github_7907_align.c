// #7907: a vector read and written through a pointer at a nondeterministic,
// suitably aligned offset into an int array.
#include <assert.h>

typedef int v4i __attribute__((__vector_size__(16)));

int __VERIFIER_nondet_int(void);

int main(void)
{
  int buf[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  int j = __VERIFIER_nondet_int();
  __VERIFIER_assume(j == 0 || j == 4);
  v4i c = *(v4i *)(buf + j);
  assert(c[3] == j + 3);
  *(v4i *)(buf + j) = (v4i){9, 9, 9, 9};
  assert(buf[j + 1] == 9);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7907_align
// EXPECT: ^VERIFICATION SUCCESSFUL$
