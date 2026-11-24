// strxfrm returns 0, not 3, when s[0] is 0.
// RUN: --unwind 8
// EXPECT: ^VERIFICATION FAILED$
#include <string.h>
extern char __VERIFIER_nondet_char(void);
extern void __VERIFIER_assert(int);
int main(void) {
  char s[4]; s[0] = __VERIFIER_nondet_char(); s[1] = 'b'; s[2] = 'c'; s[3] = 0;
  char d[8];
  __VERIFIER_assert(strxfrm(d, s, 8) == 3);
  return 0;
}
