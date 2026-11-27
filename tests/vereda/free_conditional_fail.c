// A free on one branch kills the object on that path only: the second
// free fails where the first ran, and the paths join between them.
// RUN: --force-malloc-success
// EXPECT: ^  FAILED       \[main\.invalidated-dynamic-object-freed\.1\]  line 14  dereference failure: invalidated dynamic object freed$
// EXPECT: ^VERIFICATION FAILED$
#include <stdlib.h>

int __VERIFIER_nondet_int(void);

int main(void) {
  int *p = malloc(sizeof(int));
  if (__VERIFIER_nondet_int())
    free(p);
  free(p);
  return 0;
}
