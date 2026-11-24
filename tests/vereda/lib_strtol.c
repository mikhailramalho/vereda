// strtol and its kin on "?42" and "0?1" whose character ? is nondet: base 10
// with white space, a sign or a digit first, then the octal and hex prefixes
// of base 0 and 16, and where each conversion ends.
// RUN: --unwind 8
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdlib.h>
extern char __VERIFIER_nondet_char(void);
extern void __VERIFIER_assert(int);

int main(void) {
  char c = __VERIFIER_nondet_char();
  char s[4];
  s[0] = c;
  s[1] = '4';
  s[2] = '2';
  s[3] = 0;
  char *e;
  int dig = c >= '0' && c <= '9', space = c == ' ' || (c >= '\t' && c <= '\r');
  long want = dig ? (c - '0') * 100 + 42 : c == '-' ? -42 : (c == '+' || space) ? 42 : 0;
  __VERIFIER_assert(strtol(s, &e, 10) == want);
  __VERIFIER_assert(e == (dig || space || c == '-' || c == '+' ? s + 3 : s));
  __VERIFIER_assert(strtoll(s, NULL, 10) == want);
  __VERIFIER_assert(strtoul(s, NULL, 10) == (unsigned long)want);
  __VERIFIER_assert(strtoull(s, NULL, 10) == (unsigned long long)want);

  char h[4];
  h[0] = '0';
  h[1] = c;
  h[2] = '1';
  h[3] = 0;
  int x = c == 'x' || c == 'X', oct = c >= '0' && c <= '7';
  __VERIFIER_assert(strtol(h, &e, 0) == (x ? 1 : oct ? (c - '0') * 8 + 1 : 0));
  __VERIFIER_assert(e == (x || oct ? h + 3 : h + 1));
  int hex = dig ? c - '0' : c >= 'a' && c <= 'f' ? c - 'a' + 10
            : c >= 'A' && c <= 'F' ? c - 'A' + 10 : -1;
  __VERIFIER_assert(strtol(h, &e, 16) == (x ? 1 : hex >= 0 ? hex * 16 + 1 : 0));
  __VERIFIER_assert(e == (x || hex >= 0 ? h + 3 : h + 1));
  return 0;
}
