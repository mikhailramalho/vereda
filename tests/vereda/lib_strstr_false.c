// strstr(s, "bc") is NULL when s[0] is 0, so the model must find that empty
// string for the check to fail.
// RUN: --unwind 8
// EXPECT: ^VERIFICATION FAILED$
#include <string.h>
extern char __VERIFIER_nondet_char(void);
extern void __VERIFIER_assert(int);
int main(void) {
  char s[4];
  s[0] = __VERIFIER_nondet_char();
  s[1] = 'b';
  s[2] = 'c';
  s[3] = 0;
  __VERIFIER_assert(strstr(s, "bc") != NULL);
  return 0;
}
