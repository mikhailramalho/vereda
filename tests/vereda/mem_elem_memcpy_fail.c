// Failing sibling of mem_elem_memcpy: a symbolic-offset aligned memcpy
// overwrites an element that the assertion expects to still be zero.
// EXPECT: ^VERIFICATION FAILED$
#include <string.h>
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern unsigned __VERIFIER_nondet_uint(void);

int main(void) {
  int a[8], b[2] = {10, 11};
  memset(a, 0, sizeof(a));
  unsigned s = __VERIFIER_nondet_uint();
  __VERIFIER_assume(s <= 6);
  memcpy(&a[s], b, sizeof(b));
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 8);
  __VERIFIER_assert(a[i] == 0);
  return 0;
}
