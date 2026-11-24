// strsep on "?,b" whose first character is nondet: a token, a delimiter that
// gives an empty token, or the terminator.
// RUN: --unwind 8
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>
extern char __VERIFIER_nondet_char(void);
extern void __VERIFIER_assert(int);

int main(void) {
  char s[4];
  char c = __VERIFIER_nondet_char();
  s[0] = c;
  s[1] = ',';
  s[2] = 'b';
  s[3] = 0;
  char *p = s;

  char *t = strsep(&p, ",");
  __VERIFIER_assert(t == s && p == (c == 0 ? NULL : c == ',' ? s + 1 : s + 2));

  if (c != 0 && c != ',') {
    __VERIFIER_assert(s[1] == 0);
    t = strsep(&p, ",");
    __VERIFIER_assert(t == s + 2 && p == NULL);
    __VERIFIER_assert(strsep(&p, ",") == NULL);
  }
  return 0;
}