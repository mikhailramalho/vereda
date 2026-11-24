// atoi gives 42 when s[0] is '+' or white space.
// RUN: --unwind 8
// EXPECT: ^VERIFICATION FAILED$
#include <stdlib.h>
extern char __VERIFIER_nondet_char(void);
extern void __VERIFIER_assert(int);

int main(void) {
  char s[4];
  s[0] = __VERIFIER_nondet_char();
  s[1] = '4';
  s[2] = '2';
  s[3] = 0;

  __VERIFIER_assert(atoi(s) != 42);
  return 0;
}
