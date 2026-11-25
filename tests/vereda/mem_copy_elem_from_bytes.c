// Large aligned memcpy from a byte-addressed array into an element array goes
// through Memory::copyArray at the element-array level, packing bytes from src
// in little-endian order.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern unsigned __VERIFIER_nondet_uint(void);

int main(void) {
  unsigned char src[2048];
  unsigned dst[1024];
  unsigned k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(k < 512);

  // Byte-addressed src with a symbolic write of one little-endian 32-bit word.
  src[4 * k + 0] = 0x44;
  src[4 * k + 1] = 0x33;
  src[4 * k + 2] = 0x22;
  src[4 * k + 3] = 0x11;

  dst[10] = 77;
  dst[900] = 88;
  memcpy(&dst[256], src, sizeof(src));

  __VERIFIER_assert(dst[10] == 77);
  __VERIFIER_assert(dst[900] == 88);
  __VERIFIER_assert(dst[256 + k] == 0x11223344u);
  return 0;
}
