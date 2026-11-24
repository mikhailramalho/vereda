// atoi, atol and atoll on "?42" whose first character is nondet: a digit, a
// sign, white space or anything else.
// RUN: --unwind 8
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdlib.h>
extern char __VERIFIER_nondet_char(void);
extern void __VERIFIER_assert(int);

int main(void) {
  char s[4];
  s[0] = __VERIFIER_nondet_char();
  s[1] = '4';
  s[2] = '2';
  s[3] = 0;

  int c = s[0];
  int want = (c >= '0' && c <= '9') ? (c - '0') * 100 + 42 :
             c == '-' ? -42 :
             (c == '+' || c == ' ' || (c >= '\t' && c <= '\r')) ? 42 : 0;

  __VERIFIER_assert(atoi(s) == want && atol(s) == want && atoll(s) == want);
  return 0;
}
