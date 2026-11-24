// RUN: --unwind 2
// EXPECT: ^VERIFICATION SUCCESSFUL$
// A call without a body writes only the objects it gets a pointer to, and
// printf writes none.
#include <stdio.h>
#include <unistd.h>
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);
int main(void) {
  char a[4] = "", b[4] = "", s[3] = "ab";
  int y = 3;
  int c = __VERIFIER_nondet_int();
  read(0, c ? a : b, sizeof a);
  __VERIFIER_assert(y == 3);
  if (c)
    __VERIFIER_assert(b[0] == 0);
  else
    __VERIFIER_assert(a[0] == 0);
  printf("%s\n", s);
  __VERIFIER_assert(s[0] == 'a');
  return 0;
}
