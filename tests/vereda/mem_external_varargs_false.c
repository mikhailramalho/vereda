// RUN: --unwind 2
// EXPECT: ^VERIFICATION FAILED$
// ioctl's third argument is variadic and FIONREAD writes through it.
#include <sys/ioctl.h>
extern void __VERIFIER_assert(int);
int main(void) {
  int n = 0;
  ioctl(0, FIONREAD, &n);
  __VERIFIER_assert(n == 0);
  return 0;
}
