// Bounds learned on one branch must not resolve an address on its sibling.
// EXPECT: ^VERIFICATION FAILED$
#include <stdint.h>
extern uintptr_t __VERIFIER_nondet_ulong(void);
extern void __VERIFIER_assert(int);
int a[4], b[4];
volatile uintptr_t exposed;
int main(void) {
  uintptr_t k = __VERIFIER_nondet_ulong();
  exposed = (uintptr_t)b;
  uintptr_t address = (uintptr_t)a + sizeof(int) * k;
  if (k < 4) {
    *(int *)address = 7;
    __VERIFIER_assert(a[k] == 7);
  } else {
    *(int *)address = 9;
    __VERIFIER_assert(b[0] == 0);
  }
}
