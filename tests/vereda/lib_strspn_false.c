// strcspn(s, "c") is 0, not 2, when s[0] is 'c' or 0; the model must find
// that for the check to fail.
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

  __VERIFIER_assert(strcspn(s, "c") == 2);

  return 0;
}
