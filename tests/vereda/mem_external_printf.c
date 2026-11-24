// RUN: --unwind 2
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Without a %n, printf only reads its variadic arguments.
#include <stdio.h>
extern void __VERIFIER_assert(int);
int main(void) {
  char s[4] = "abc";
  int n = 0;
  printf("%s %5.2d %%n\n", s, n);
  __VERIFIER_assert(s[0] == 'a');
  return 0;
}
