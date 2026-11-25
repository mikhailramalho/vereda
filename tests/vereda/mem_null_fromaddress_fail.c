// An integer-provenance pointer loaded from memory can be non-null when the
// stored integer is non-zero, and a wild out-of-bounds pointer at address 0
// is not equal to NULL.
// EXPECT: ^VERIFICATION FAILED$
#include <stdint.h>

extern void __VERIFIER_assert(int);
extern uintptr_t __VERIFIER_nondet_ulong(void);

int main(void) {
  uintptr_t raw = __VERIFIER_nondet_ulong();
  void *buf[1];
  *(uintptr_t *)&buf[0] = raw;
  void *loaded = buf[0];
  if (raw != 0)
    __VERIFIER_assert(loaded == 0);
  return 0;
}
