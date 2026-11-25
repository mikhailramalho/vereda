// Unaligned memcpy (offset or length not a multiple of the element size) keeps
// the array on bytes and preserves exact little-endian byte contents.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern unsigned __VERIFIER_nondet_uint(void);

int main(void) {
  unsigned a[4] = {0, 0, 0, 0};
  unsigned b[2] = {0x11223344u, 0x55667788u};

  // Unaligned offset (2 bytes into a[0]), aligned length (4 bytes).
  memcpy((char *)a + 2, b, 4);
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 2);
  __VERIFIER_assert(a[0] == 0x33440000u);
  __VERIFIER_assert(a[1] == 0x00001122u);
  __VERIFIER_assert(a[i] == (i == 0 ? 0x33440000u : 0x00001122u));

  // Aligned offset, unaligned length (2 bytes).
  unsigned c[2] = {0, 0};
  memcpy(c, b, 2);
  __VERIFIER_assert(c[i] == (i == 0 ? 0x00003344u : 0u));
  return 0;
}
