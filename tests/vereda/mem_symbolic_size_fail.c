// Failing counterpart for symbolic Memory::size: an object allocated after
// lazy sizes array creation has size 48, not 32.
// EXPECT: ^VERIFICATION FAILED$
#include <stddef.h>

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
  (void)__vereda_object_size(ptrs1[i]);

  char d[48];
  void *ptrs2[3] = {a, d, c};
  unsigned j = __VERIFIER_nondet_uint();
  __VERIFIER_assume(j < 3);
  size_t s2 = __vereda_object_size(ptrs2[j]);
  if (j == 1)
    __VERIFIER_assert(s2 == 32);
  return 0;
}
