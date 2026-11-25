// Null comparisons on fromAddress() results and loaded pointers simplify
// without losing soundness, including for a wild out-of-bounds pointer whose
// numeric address wraps to 0.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdint.h>
#include <stdlib.h>

extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);
extern uintptr_t __VERIFIER_nondet_ulong(void);

int main(void) {
  // Constant address 0 converts directly to null.
  int *z = (int *)(uintptr_t)0;
  __VERIFIER_assert(z == 0);

  // Integer-provenance value loaded from memory as a pointer.
  uintptr_t raw = __VERIFIER_nondet_ulong();
  void *buf[1];
  *(uintptr_t *)&buf[0] = raw;
  void *loaded = buf[0];
  __VERIFIER_assert((loaded == 0) == (raw == 0));

  // Wild out-of-bounds pointer whose address is 0 still has non-zero object id.
  int x = 1;
  char *wild = (char *)&x - (uintptr_t)&x;
  char *slot = __VERIFIER_nondet_int() ? wild : (char *)0;
  char * volatile v = slot;
  char *q = v;
  if (slot == wild)
    __VERIFIER_assert(q != 0);
  else
    __VERIFIER_assert(q == 0);
  return 0;
}
