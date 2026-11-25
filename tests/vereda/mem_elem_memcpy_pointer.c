// Copying pointers through an integer array via memcpy must keep the array on
// bytes so pointer provenance survives the round trip.
// EXPECT: ^VERIFICATION SUCCESSFUL$
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
  // Access mid at a symbolic index as uintptr_t so it would be an element
  // array if memcpy did not check pointer provenance.
  mid[i] = 0;

  memcpy(mid, src, sizeof(mid));
  memcpy(dst, mid, sizeof(dst));
  __VERIFIER_assert(*dst[i] == (i == 0 ? 42 : 99));
  return 0;
}
