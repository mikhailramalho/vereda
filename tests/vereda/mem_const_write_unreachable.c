// The same writes into a const object and a string literal, on paths that
// cannot happen: x is below 10, and the pointer is the literal only when x
// is negative. The check is guarded as the path is, so neither fails.
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT-NOT: FAILED
#include <string.h>

extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);

const int limit = 10;

int main(void) {
  char buf[4];
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x >= 0 && x < 10);
  char *p = x < 0 ? "abc" : buf;
  memset(p, 0, 2);
  p[1] = 'x';
  if (x > 20)
    *(int *)&limit = 5;
  return 0;
}
