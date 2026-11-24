// RUN: --unwind 2
// EXPECT: ^VERIFICATION FAILED$
// A %n before any conversion is written even when sscanf returns EOF.
#include <stdio.h>
extern void __VERIFIER_assert(int);
int main(void) {
  int n = -1, x;
  char s[2] = "";
  if (sscanf(s, "%n%d", &n, &x) == EOF)
    __VERIFIER_assert(n == -1);
  return 0;
}
