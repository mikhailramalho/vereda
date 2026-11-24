// strcasecmp and strncasecmp on a string whose first character is nondet; "abd"
// differs from it only after a difference of case, which must not end the compare.
// RUN: --unwind 8
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <strings.h>
extern char __VERIFIER_nondet_char(void);
extern void __VERIFIER_assert(int);

int main(void) {
  char s[4];
  s[0] = __VERIFIER_nondet_char();
  s[1] = 'B';
  s[2] = 'c';
  s[3] = 0;

  __VERIFIER_assert((strcasecmp(s, "abc") == 0) == (s[0] == 'a' || s[0] == 'A'));
  if (s[0] == 0)
    __VERIFIER_assert(strcasecmp(s, "abc") < 0);
  __VERIFIER_assert(strcasecmp(s, "abd") != 0);
  __VERIFIER_assert((strncasecmp(s, "xbz", 2) == 0) == (s[0] == 'x' || s[0] == 'X'));

  return 0;
}
