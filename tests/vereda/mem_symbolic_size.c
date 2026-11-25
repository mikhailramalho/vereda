// Symbolic Memory::size queries via __vereda_object_size and realloc: both
// objects allocated before the first symbolic query (lazy sizes array init)
// and objects allocated after it report exact sizes.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stddef.h>
#include <stdlib.h>

extern size_t __vereda_object_size(const void *);
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern unsigned __VERIFIER_nondet_uint(void);

int main(void) {
  char a[16];
  char b[32];
  char c[64];
  void *ptrs1[3] = {a, b, c};

  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 3);
  void *p = ptrs1[i];
  size_t s1 = __vereda_object_size(p);
  if (i == 0)
    __VERIFIER_assert(s1 == 16);
  else if (i == 1)
    __VERIFIER_assert(s1 == 32);
  else
    __VERIFIER_assert(s1 == 64);

  // Allocate after sizes array has been lazily created.
  char d[48];
  void *ptrs2[3] = {a, d, c};
  unsigned j = __VERIFIER_nondet_uint();
  __VERIFIER_assume(j < 3);
  void *q = ptrs2[j];
  size_t s2 = __vereda_object_size(q);
  if (j == 0)
    __VERIFIER_assert(s2 == 16);
  else if (j == 1)
    __VERIFIER_assert(s2 == 48);
  else
    __VERIFIER_assert(s2 == 64);

  return 0;
}
