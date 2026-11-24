// RUN: --unwind 8
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>
extern char __VERIFIER_nondet_char(void);
extern void __VERIFIER_assert(int);
int main(void) {
  char s[4]; s[0] = __VERIFIER_nondet_char(); s[1] = 'b'; s[2] = 'c'; s[3] = 0;
  int c = strcoll(s, "abc"), m = strcmp(s, "abc");
  __VERIFIER_assert((c < 0) == (m < 0) && (c == 0) == (m == 0));
  char d[8];
  __VERIFIER_assert(strxfrm(d, s, 8) == (s[0] == 0 ? 0 : 3) && strcmp(d, s) == 0);
  __VERIFIER_assert(strxfrm(NULL, s, 0) == strlen(s));
  return 0;
}