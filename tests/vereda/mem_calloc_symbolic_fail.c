// EXPECT: ^VERIFICATION FAILED$
// A write into a calloc'd object of nondet size reaches the zeroed array:
// a read at another nondet index may see it.
#include <stdlib.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  unsigned n = __VERIFIER_nondet_uint(), i = __VERIFIER_nondet_uint(),
           j = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n < 100 && i < n && j < n);
  char *p = calloc(n, 1);
  if (p) {
    p[i] = 1;
    __VERIFIER_assert(p[j] == 0);
  }
  return 0;
}
