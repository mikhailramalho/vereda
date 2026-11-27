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
  assert(buf[1] == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10891_write_offset_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
