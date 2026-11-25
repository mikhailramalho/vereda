// Choices retain their offset, both targets, and the null alternative.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdint.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int a[4], b[4];
int main(void) {
  unsigned k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(k < 4);
  int c = __VERIFIER_nondet_int(), valid = __VERIFIER_nondet_int();
  uintptr_t base = c ? (uintptr_t)a : (uintptr_t)b;
  int *p = (int *)(valid ? base + sizeof(int) * k : 0);
  __VERIFIER_assert((p == 0) == !valid);
  if (valid) {
    *p = 7;
    __VERIFIER_assert(c ? a[k] == 7 && b[k] == 0 : b[k] == 7 && a[k] == 0);
  }
}
