// index(s, 'c') is s when s[0] is 'c' and NULL when it is 0, not s + 2.
// RUN: --unwind 8
// EXPECT: ^VERIFICATION FAILED$
#include <string.h>
#include <strings.h>
extern char __VERIFIER_nondet_char(void);
extern void __VERIFIER_assert(int);

int main(void) {
  char s[4]; s[0] = __VERIFIER_nondet_char(); s[1] = 'b'; s[2] = 'c'; s[3] = 0;
  __VERIFIER_assert(index(s, 'c') == s + 2);
  return 0;
}
