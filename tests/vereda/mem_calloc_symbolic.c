// RUN: --force-malloc-success
// EXPECT: ^VERIFICATION SUCCESSFUL$
// calloc with a size that has many values on the path: its object starts
// as a constant array of zeros, so every byte reads back as zero.
#include <stdlib.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  unsigned n = __VERIFIER_nondet_uint(), i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n > 0 && n < 100 && i < n);
  int *p = calloc(n, sizeof(int));
  __VERIFIER_assert(p[i] == 0);
  p[i] = 5;
  __VERIFIER_assert(p[i] == 5 && p[0] + p[n - 1] <= 10);
  return 0;
}
