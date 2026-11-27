// aligned_alloc promises the alignment asked for, not twice it.
// EXPECT: ^  FAILED       \[main\.assertion\.1\]  line 11  __VERIFIER_assert$
// EXPECT: ^VERIFICATION FAILED$
#include <stdint.h>
#include <stdlib.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assert(int);
int main(void) {
  size_t a = (size_t)8 << (__VERIFIER_nondet_uint() % 4);
  char *b = aligned_alloc(a, 24);
  __VERIFIER_assert(((uintptr_t)b & (2 * a - 1)) == 0);
  free(b);
  return 0;
}
