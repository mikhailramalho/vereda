/* #10891 */
#include <assert.h>
#include <stdint.h>
unsigned __VERIFIER_nondet_uint(void);
struct __attribute__((packed)) U { uint32_t v; };
int main(void)
{
  uint32_t buf[2] = {0, 0};
  unsigned o = __VERIFIER_nondet_uint();
  __VERIFIER_assume(o == 1);
  struct U val = {0xAABBCCDDu};
  *(struct U *)((unsigned char *)buf + o) = val;
  assert(buf[0] == 0xBBCCDD00u);
  assert(buf[1] == 0x000000AAu);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10891_write_offset
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
