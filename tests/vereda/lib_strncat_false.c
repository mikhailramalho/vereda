// strncat appends nothing when s[0] is 0, so d keeps length 1.
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

  char d[8] = {'x', 0};
  strncat(d, s, 2);

  __VERIFIER_assert(strlen(d) == 3);

  return 0;
}
