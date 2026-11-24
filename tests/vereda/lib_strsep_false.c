// strsep leaves p at s + 1 when s[0] is ',' and sets it to NULL when s[0] is 0.
// RUN: --unwind 8
// EXPECT: ^VERIFICATION FAILED$
#include <string.h>
extern char __VERIFIER_nondet_char(void);
extern void __VERIFIER_assert(int);

int main(void) {
  char s[4];
  char c = __VERIFIER_nondet_char();
  s[0] = c;
  s[1] = ',';
  s[2] = 'b';
  s[3] = 0;
  char *p = s;

  strsep(&p, ",");
  __VERIFIER_assert(p == s + 2);
  return 0;
}