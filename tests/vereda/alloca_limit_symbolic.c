// EXPECT: ^VERIFICATION FAILED$
// EXPECT: FAILED +\[main\.assertion\.1\].*alloca: size exceeds PTRDIFF_MAX
// EXPECT-NOT: FAILED +\[main\.assertion\.2\]
#include <stddef.h>
#include <stdint.h>
extern unsigned long __VERIFIER_nondet_ulong(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  size_t n = __VERIFIER_nondet_ulong();
  __VERIFIER_assume(n > (size_t)PTRDIFF_MAX / sizeof(int));
  volatile int a[n];
  a[0] = 1;
  __VERIFIER_assert(0); // no execution reaches this after a failing allocation
}
