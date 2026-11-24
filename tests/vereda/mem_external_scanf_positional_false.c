// RUN: --unwind 2
// EXPECT: ^VERIFICATION FAILED$
// %1$d names its argument by position: the walker does not follow
// positions, so it writes each whole object, and "%1$d %1$d" assigns two.
#include <stdio.h>
extern void __VERIFIER_assert(int);
int main(void) {
  char s[8];
  int x = 0;
  if (sscanf(s, "%1$d %1$d", &x) == 2)
    __VERIFIER_assert(0);
  return 0;
}
