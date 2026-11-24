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

  __VERIFIER_assert(strcasecmp(s, "abc") != 0); // This is wrong only when s[0] is 'a' or 'A'

  return 0;
}