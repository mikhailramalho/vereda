// Failing sibling of mem_elem_memcpy_unaligned: an unaligned memcpy modifies
// the upper bytes of a[0] and lower bytes of a[1].
// EXPECT: ^VERIFICATION FAILED$
#include <string.h>
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern unsigned __VERIFIER_nondet_uint(void);

int main(void) {
  unsigned a[4] = {0, 0, 0, 0};
  unsigned b[1] = {0x11223344u};
  memcpy((char *)a + 2, b, 4);
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 4);
  __VERIFIER_assert(a[i] == 0);
  return 0;
}
