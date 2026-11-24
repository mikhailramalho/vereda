// The BSD string models: index, rindex, bzero, bcopy and memccpy - false assertions.
// RUN: --unwind 8
// EXPECT: ^VERIFICATION FAILED$
#include <string.h>
#include <strings.h>
extern char __VERIFIER_nondet_char(void);
extern void __VERIFIER_assert(int);

int main(void) {
  char s[4]; s[0] = __VERIFIER_nondet_char(); s[1] = 'b'; s[2] = 'c'; s[3] = 0;
  // This assertion should fail - index(s, 'c') is wrong when s[0] is 'c' or 0
  __VERIFIER_assert(index(s, 'c') == s + 2);
  return 0;
}