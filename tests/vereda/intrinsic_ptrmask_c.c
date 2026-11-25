// Pointer tagging in C: masking off low tag bits on an aligned object
// recovers the original pointer and reads the stored value.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdint.h>

extern void __VERIFIER_assert(int);

static int *mask_ptr(int *p) {
  return (int *)((uintptr_t)p & ~(uintptr_t)7);
}

int main(void) {
  int x __attribute__((aligned(8))) = 42;
  int *p = (int *)((char *)&x + 3);
  int *q = mask_ptr(p);
  __VERIFIER_assert(*q == 42);
  return 0;
}
