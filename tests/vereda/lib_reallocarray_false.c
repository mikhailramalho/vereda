// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  FAILED +\[main\.invalidated-dynamic-object-freed\.1\]
// reallocarray frees p where n * 8 does not overflow and the new
// allocation succeeds, so freeing p again is a double free there (#358).
#include <stdlib.h>
size_t __VERIFIER_nondet_ulong(void);
int main(void) {
  int *p = malloc(sizeof(int));
  int *q = reallocarray(p, __VERIFIER_nondet_ulong(), 8);
  free(p);
  free(q);
  return 0;
}
