// RUN: -m32 --unwind 2
// EXPECT: ^VERIFICATION FAILED$
// glibc's %Ld converts into a long long, all 8 bytes of it, on a 32-bit
// target too.
#include <stdio.h>
extern void __VERIFIER_assert(int);
int main(void) {
  long long x = 0;
  scanf("%Ld", &x);
  __VERIFIER_assert((x >> 32) == 0);
  return 0;
}
