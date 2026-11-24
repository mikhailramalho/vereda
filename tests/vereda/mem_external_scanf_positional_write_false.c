// RUN: --unwind 2
// EXPECT: ^VERIFICATION FAILED$
// %1$d writes all of x, not the two bytes of a width-1 unknown conversion.
#include <stdio.h>
extern void __VERIFIER_assert(int);
int main(void) {
  char s[8];
  int x = 0;
  sscanf(s, "%1$d", &x);
  __VERIFIER_assert(x < 65536);
  return 0;
}
