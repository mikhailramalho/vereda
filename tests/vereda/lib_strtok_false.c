// strtok(s, ",") skips a leading ',' and finds no token when s[0] is 0.
// RUN: --unwind 8
// EXPECT: ^VERIFICATION FAILED$
#include <string.h>
extern char __VERIFIER_nondet_char(void);
extern void __VERIFIER_assert(int);

int main(void) {
  char s[4];
  s[0] = __VERIFIER_nondet_char();
  s[1] = ',';
  s[2] = 'b';
  s[3] = 0;
  __VERIFIER_assert(strtok(s, ",") == s);
  return 0;
}
