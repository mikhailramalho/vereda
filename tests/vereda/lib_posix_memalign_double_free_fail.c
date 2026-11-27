// posix_memalign's object comes from malloc, so freeing it twice is a double
// free.
// EXPECT: ^  dereference failure: invalidated dynamic object freed$
// EXPECT: ^  FAILED       \[main\.invalidated-dynamic-object-freed\.1\]  line 14  dereference failure: invalidated dynamic object freed$
// EXPECT: ^VERIFICATION FAILED$
#include <stdlib.h>
extern unsigned __VERIFIER_nondet_uint(void);
int main(void) {
  size_t a = (size_t)8 << (__VERIFIER_nondet_uint() % 4);
  void *p;
  if (posix_memalign(&p, a, 24))
    return 0;
  free(p);
  free(p);
  return 0;
}
