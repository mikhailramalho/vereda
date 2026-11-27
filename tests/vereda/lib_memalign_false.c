// memalign does not fail on an alignment that is not a power of two, as
// glibc's does not: the object exists, so the assertion that it does not
// fails.
// EXPECT: ^  FAILED       \[main\.assertion\.1\]  line 13  __VERIFIER_assert$
// EXPECT: ^VERIFICATION FAILED$
#include <stdlib.h>
void *memalign(size_t, size_t);
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assert(int);
int main(void) {
  size_t n = __VERIFIER_nondet_uint() % 16 + 1;
  char *p = memalign(24, n);
  __VERIFIER_assert(!p);
  free(p);
  return 0;
}
