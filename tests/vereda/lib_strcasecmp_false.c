// strcasecmp(s, "abc") is 0 when s[0] is 'a' or 'A'.
// RUN: --unwind 8
// EXPECT: ^VERIFICATION FAILED$
#include <strings.h>
extern char __VERIFIER_nondet_char(void);
extern void __VERIFIER_assert(int);

int main(void) {
  char s[4];
  s[0] = __VERIFIER_nondet_char();
  s[1] = 'B';
  s[2] = 'c';
  s[3] = 0;

  __VERIFIER_assert(strcasecmp(s, "abc") != 0);

  return 0;
}
