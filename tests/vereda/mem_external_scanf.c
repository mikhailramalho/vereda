// RUN: --unwind 2
// EXPECT: ^VERIFICATION SUCCESSFUL$
// scanf writes only the arguments its format converts into, each with the
// bytes of its type, and only the first r it returns. stdin is an object of
// its own, even when read after a store at a symbolic address.
#include <stdio.h>
extern void __VERIFIER_assert(int);
extern unsigned __VERIFIER_nondet_uint(void);
struct s { char c, d; short h; } g = {1, 2, 3};
int main(void) {
  int a = 1, b = 2, c = 3, n = -1;
  char s[4] = "";
  s[__VERIFIER_nondet_uint() % 4] = 'x';
  scanf("%d %*d %%", &a, &b);
  fscanf(stdin, "%3s", s, &c);
  __VERIFIER_assert(b == 2 && c == 3);
  int r = sscanf(s, "%hhd%n", &g.d, &n);
  __VERIFIER_assert(g.c == 1 && g.h == 3);
  if (r < 1)
    __VERIFIER_assert(g.d == 2 && n == -1);
  return 0;
}
