// A local-linkage global whose address is exposed (even after fromAddress is
// called on a nondet integer) can still be aliased through that pointer.
// EXPECT: ^VERIFICATION FAILED$
#include <stdint.h>

extern void __VERIFIER_assert(int);
extern uintptr_t __VERIFIER_nondet_ulong(void);

static int target = 10;

int main(void) {
  uintptr_t a = __VERIFIER_nondet_ulong();
  int *p = (int *)a;
  if (p == &target) {
    *p = 20;
    __VERIFIER_assert(target == 10);
  }
  return 0;
}
