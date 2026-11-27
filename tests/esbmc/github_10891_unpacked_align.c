/* #10891 */
#include <assert.h>
#include <stdint.h>
unsigned __VERIFIER_nondet_uint(void);
struct H { uint16_t a, b; };
int main(void)
{
  uint32_t buf[2] = {0x00020001u, 0x00040003u};
  unsigned o = __VERIFIER_nondet_uint();
  __VERIFIER_assume(o <= 4 && o % 2 == 0);
  struct H r = *(struct H *)((unsigned char *)buf + o);
  assert(o != 2 || (r.a == 2 && r.b == 3));
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10891_unpacked_align
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
