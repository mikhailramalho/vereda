// Failing counterpart for pointer loads with symbolic isInt: the integer
// branch loads &y, not &x.
// EXPECT: ^VERIFICATION FAILED$
#include <stdint.h>

extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);

int main(void) {
  int x = 11, y = 22;
  int c = __VERIFIER_nondet_int();
  int *slot;
  if (c)
    slot = &x;
  else
    *(uintptr_t *)&slot = (uintptr_t)&y;
  int *p = slot;
  __VERIFIER_assert(*p == 11);
  return 0;
}
