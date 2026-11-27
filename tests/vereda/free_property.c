// --property valid-free counts only invalid frees, and prints the SV-COMP
// verdict as ESBMC's wrapper does. The assertion does not count: where it
// fails the path ends, and the double free is found where it holds.
// RUN: --property valid-free
// EXPECT-NOT: __VERIFIER_assert
// EXPECT: ^  FAILED       \[main\.invalidated-dynamic-object-freed\.1\]  line 19  dereference failure: invalidated dynamic object freed$
// EXPECT: ^FALSE_FREE$
#include <stdlib.h>

void __VERIFIER_assert(int);
int __VERIFIER_nondet_int(void);

int main(void) {
  int *p = malloc(sizeof(int));
  int n = __VERIFIER_nondet_int();
  __VERIFIER_assert(n);
  free(p);
  if (n == 1)
    free(p);
  return 0;
}
