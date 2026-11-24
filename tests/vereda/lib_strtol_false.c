// strtol reads nothing from "?42" when ? is not a digit, a sign or white
// space, and then leaves the end at the string's start.
// RUN: --unwind 8
// EXPECT: ^VERIFICATION FAILED$
#include <stdlib.h>
extern char __VERIFIER_nondet_char(void);
extern void __VERIFIER_assert(int);

int main(void) {
  char s[4];
  s[0] = __VERIFIER_nondet_char();
  s[1] = '4';
  s[2] = '2';
  s[3] = 0;
  char *e;
  strtol(s, &e, 10);
  __VERIFIER_assert(e == s + 3);
  return 0;
}
