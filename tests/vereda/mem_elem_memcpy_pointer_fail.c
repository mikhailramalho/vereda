// Failing sibling of mem_elem_memcpy_pointer: provenance survives memcpy
// through an integer array, so writing through the recovered pointer updates x.
// EXPECT: ^VERIFICATION FAILED$
#include <stdint.h>
#include <string.h>
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern unsigned __VERIFIER_nondet_uint(void);

int main(void) {
  int x = 42, y = 99;
  int *src[2] = {&x, &y};
  uintptr_t mid[2];
  int *dst[2];

  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 2);
  mid[i] = 0;

  memcpy(mid, src, sizeof(mid));
  memcpy(dst, mid, sizeof(dst));
  *dst[i] = 7;
  __VERIFIER_assert(x == 42);
  return 0;
}
