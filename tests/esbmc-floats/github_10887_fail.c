// #10887: flipping a payload bit of a NaN held in a float changes its bits.
#include <assert.h>
#include <stdint.h>
#include <string.h>

unsigned __VERIFIER_nondet_uint(void);

int main(void)
{
  uint32_t x = __VERIFIER_nondet_uint();
  __VERIFIER_assume((x & 0x7f800000u) == 0x7f800000u && (x & 0x007fffffu) > 1);
  float f;
  memcpy(&f, &x, sizeof f);
  ((unsigned char *)&f)[0] ^= 1;
  uint32_t y;
  memcpy(&y, &f, sizeof y);
  assert(x == y);
  return 0;
}
// Imported from ESBMC regression/floats/github_10887_fail
// EXPECT: ^VERIFICATION FAILED$
