// RUN: --unwind 12
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>
extern size_t strlcpy(char *, const char *, size_t);
extern char __VERIFIER_nondet_char(void);
extern void __VERIFIER_assert(int);

int main(void) {
  char c = __VERIFIER_nondet_char();
  char s[4] = {c, 'b', 'c', 0};
  char a[5] = {'x', 'x', 'x', 'x', 'x'};

  size_t n = strlcpy(a, s, 3);
  __VERIFIER_assert(n == (c == 0 ? 0 : 3));
  __VERIFIER_assert(a[0] == c);
  __VERIFIER_assert(a[1] == (c == 0 ? 'x' : 'b'));
  __VERIFIER_assert(a[2] == (c == 0 ? 'x' : 0));
  __VERIFIER_assert(a[3] == 'x');

  char b[2] = {'x', 'y'};
  volatile size_t zero = 0;
  __VERIFIER_assert(strlcpy(b, s, zero) == (c == 0 ? 0 : 3));
  __VERIFIER_assert(b[0] == 'x');
  __VERIFIER_assert(b[1] == 'y');

  char d[5] = {'x', 'x', 'x', 'x', 'x'};
  volatile size_t five = 5;
  __VERIFIER_assert(strlcpy(d, s, five) == (c == 0 ? 0 : 3));
  __VERIFIER_assert(d[0] == c);
  if (c != 0) {
    __VERIFIER_assert(d[1] == 'b');
    __VERIFIER_assert(d[2] == 'c');
    __VERIFIER_assert(d[3] == 0);
  }
  char e[2] = {'x', 'y'};
  volatile size_t one = 1;
  __VERIFIER_assert(strlcpy(e, s, one) == (c == 0 ? 0 : 3));
  __VERIFIER_assert(e[0] == 0 && e[1] == 'y');

  return 0;
}
