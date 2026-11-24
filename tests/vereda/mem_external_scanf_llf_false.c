// RUN: --unwind 2
// EXPECT: ^VERIFICATION FAILED$
// glibc reads %llf as %Lf: a long double, past the 8 bytes of a double.
#include <stdio.h>
extern void __VERIFIER_assert(int);
int main(void) {
  char s[8];
  long double x;
  unsigned char *b = (unsigned char *)&x;
  b[9] = 0;
  sscanf(s, "%llf", &x);
  __VERIFIER_assert(b[9] == 0);
  return 0;
}
