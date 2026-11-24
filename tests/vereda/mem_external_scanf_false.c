// RUN: --unwind 2
// EXPECT: ^VERIFICATION FAILED$
// fscanf writes each argument its format converts into.
#include <stdio.h>
extern void __VERIFIER_assert(int);
int main(void) {
  int a = 1, b = 2;
  fscanf(stdin, "%d %5d", &a, &b);
  __VERIFIER_assert(b == 2);
  return 0;
}
