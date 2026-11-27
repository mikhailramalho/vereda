// Shrunk below n, the copy stops before p[n - 1]: q[m - 1] is another byte.
// EXPECT: ^  FAILED       \[main\.assertion\.1\]  line 19  __VERIFIER_assert$
// EXPECT: ^VERIFICATION FAILED$
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
  p[n - 1] = 'z';
  char *q = realloc(p, m);
  if (!q)
    return 0;
  __VERIFIER_assert(q[m - 1] == 'z');
  return 0;
}
