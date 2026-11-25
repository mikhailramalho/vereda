// Failing sibling of mem_copy_elem_from_bytes: a byte copied from a
// byte-addressed source into an element array changes the packed element.
// EXPECT: ^VERIFICATION FAILED$
#include <string.h>
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern unsigned __VERIFIER_nondet_uint(void);

int main(void) {
  unsigned char src[2048];
  unsigned dst[1024];
  unsigned k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(k < 512);

  src[4 * k + 0] = 0x44;
  src[4 * k + 1] = 0x33;
  src[4 * k + 2] = 0x22;
  src[4 * k + 3] = 0x11;

  memcpy(&dst[256], src, sizeof(src));
  __VERIFIER_assert(dst[256 + k] == 0x11223345u);
  return 0;
}
