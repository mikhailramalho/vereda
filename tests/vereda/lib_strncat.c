// strncat appends at most n characters of a string whose first character is
// nondet.
// RUN: --unwind 8
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>
extern char __VERIFIER_nondet_char(void);
extern void __VERIFIER_assert(int);

int main(void) {
  char s[4];
  s[0] = __VERIFIER_nondet_char();
  s[1] = 'b';
  s[2] = 'c';
  s[3] = 0;

  char d[8] = {'x', 0};
  char *r = strncat(d, s, 2);

  __VERIFIER_assert(r == d);
  __VERIFIER_assert(strlen(d) == (s[0] == 0 ? 1 : 3));

  if (s[0] != 0) {
    __VERIFIER_assert(d[1] == s[0]);
    __VERIFIER_assert(d[2] == 'b');
    __VERIFIER_assert(d[3] == 0);
  }

  return 0;
}
