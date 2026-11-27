// posix_memalign, aligned_alloc, memalign and valloc allocate through malloc,
// so free takes their results, at an address aligned as asked; a bad
// alignment gives EINVAL and leaves *memptr alone.
// RUN: -m64
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
extern void *memalign(size_t, size_t);
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assert(int);
int main(void) {
  size_t a = (size_t)8 << (__VERIFIER_nondet_uint() % 4);
  void *p = 0, *q = 0;
  int r = posix_memalign(&p, a, 24);
  __VERIFIER_assert(r == 0 || r == ENOMEM);
  __VERIFIER_assert(r == 0 || p == 0);
  __VERIFIER_assert(r != 0 || ((uintptr_t)p & (a - 1)) == 0);
  __VERIFIER_assert(posix_memalign(&q, 3 * a, 8) == EINVAL && q == 0);
  __VERIFIER_assert(posix_memalign(&q, 4, 8) == EINVAL && q == 0);
  char *b = aligned_alloc(a, 24);
  __VERIFIER_assert(((uintptr_t)b & (a - 1)) == 0);
  errno = 0;
  __VERIFIER_assert(aligned_alloc(3 * a, 8) == 0 && errno == EINVAL);
  __VERIFIER_assert(aligned_alloc(0, 8) == 0);
  char *m = memalign(a, 10);
  __VERIFIER_assert(((uintptr_t)m & (a - 1)) == 0);
  char *v = valloc(10);
  __VERIFIER_assert(((uintptr_t)v & 4095) == 0);
  free(p);
  free(b);
  free(m);
  free(v);
  return 0;
}
