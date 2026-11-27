// q points to a or b by a nondet choice, and a callee frees a: a stays
// dead after the return, and the free of q, read back from memory, fails
// where q is a.
// RUN: --force-malloc-success
// EXPECT: ^  FAILED       \[main\.invalidated-dynamic-object-freed\.1\]  line 19  dereference failure: invalidated dynamic object freed$
// EXPECT: ^VERIFICATION FAILED$
#include <stdlib.h>

int __VERIFIER_nondet_int(void);

void release(int *p) { free(p); }

int main(void) {
  int *a = malloc(sizeof(int));
  int *b = malloc(sizeof(int));
  int *q = __VERIFIER_nondet_int() ? a : b;
  release(a);
  int *volatile slot = q;
  free(slot);
  return 0;
}
