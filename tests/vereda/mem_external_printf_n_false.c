// RUN: --unwind 2
// EXPECT: ^VERIFICATION FAILED$
// printf writes the count of characters through %n's argument.
#include <stdio.h>
extern void __VERIFIER_assert(int);
int main(void) {
  int n = 0;
  printf("abc%n", &n);
  __VERIFIER_assert(n == 0);
  return 0;
}
