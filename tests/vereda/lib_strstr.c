// strstr on a string whose first character is nondet, so that clang cannot fold
// the calls and the model runs; for s[0] == 'b' the string is "bbc", for 'c' "cbc".
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
  __VERIFIER_assert(strstr(s, "bc") == (s[0] == 0 ? NULL : s + 1));
  __VERIFIER_assert(strstr(s, "ab") == (s[0] == 'a' ? s : NULL));
  __VERIFIER_assert(strstr(s, "") == s);
  __VERIFIER_assert(strstr(s, "cb") == (s[0] == 'c' ? s : NULL));
  return 0;
}
