// strtok and strtok_r on ",?b" whose first character is nondet: a token, a
// delimiter that strtok skips, or the terminator.
// RUN: --unwind 8
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>
extern char __VERIFIER_nondet_char(void);
extern void __VERIFIER_assert(int);

int main(void) {
  char c = __VERIFIER_nondet_char();
  char s[4];
  s[0] = c;
  s[1] = ',';
  s[2] = 'b';
  s[3] = 0;
  char *t = strtok(s, ",");
  __VERIFIER_assert(t == (c == 0 ? NULL : c == ',' ? s + 2 : s));
  if (c != 0 && c != ',') {
    __VERIFIER_assert(s[1] == 0);
    __VERIFIER_assert(strtok(NULL, ",") == s + 2);
    __VERIFIER_assert(strtok(NULL, ",") == NULL);
  }

  char r[4];
  r[0] = c;
  r[1] = ',';
  r[2] = 'b';
  r[3] = 0;
  char *save;
  t = strtok_r(r, ",", &save);
  __VERIFIER_assert(t == (c == 0 ? NULL : c == ',' ? r + 2 : r));
  if (c != 0 && c != ',') {
    __VERIFIER_assert(r[1] == 0);
    __VERIFIER_assert(strtok_r(NULL, ",", &save) == r + 2);
    __VERIFIER_assert(strtok_r(NULL, ",", &save) == NULL);
  }
  return 0;
}
