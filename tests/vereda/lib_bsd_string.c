// The BSD string models: index, rindex, bzero, bcopy and memccpy.
// RUN: --unwind 8
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>
#include <strings.h>
extern char __VERIFIER_nondet_char(void);
extern void __VERIFIER_assert(int);

int main(void) {
  char s[4]; s[0] = __VERIFIER_nondet_char(); s[1] = 'b'; s[2] = 'c'; s[3] = 0;
  __VERIFIER_assert(index(s, 'c') == (s[0] == 'c' ? s : s[0] == 0 ? NULL : s + 2));
  __VERIFIER_assert(rindex(s, 'c') == (s[0] == 0 ? NULL : s + 2));

  char b[4]; b[0] = __VERIFIER_nondet_char(); b[1] = __VERIFIER_nondet_char(); b[2] = 'c'; b[3] = 0;
  bzero(b, 2);
  __VERIFIER_assert(b[0] == 0 && b[1] == 0 && b[2] == 'c');

  bcopy(s, b, 3);
  __VERIFIER_assert(b[0] == s[0] && b[1] == 'b' && b[2] == 'c');

  char m[4];
  char *e = memccpy(m, s, 'b', 4);
  __VERIFIER_assert(e == (s[0] == 'b' ? m + 1 : m + 2) && m[0] == s[0]);
  return 0;
}