// realloc to a nondet size copies the old contents into an object of that
// size: a memcpy of min(old, new) bytes, neither a constant.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdlib.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

int main(void) {
  unsigned n = __VERIFIER_nondet_uint(), m = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n >= 2 && n <= 100 && m >= 1 && m <= 200);
  char *p = malloc(n);
  if (!p)
    return 0;
  p[0] = 'a';
  p[n - 1] = 'z';
  char *q = realloc(p, m);
  if (!q)
    return 0;
  __VERIFIER_assert(q[0] == 'a');
  __VERIFIER_assert(m < n || q[n - 1] == 'z');
  return 0;
}
