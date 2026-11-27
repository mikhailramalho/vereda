/* As github_10939_struct_src, but asserting the high byte is first. */
#include <assert.h>
#include <stddef.h>
struct cannot_drop { unsigned int c; };
union chunk { unsigned char bytes[4]; };
size_t __VERIFIER_nondet_size(void);
int main(void)
{
  struct cannot_drop x = {0x11223344u};
  size_t off = __VERIFIER_nondet_size();
  __VERIFIER_assume(off == 0);
  union chunk a = *(union chunk *)((unsigned char *)&x + off);
  assert(a.bytes[0] == 0x11);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10939_struct_src_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^Properties: [0-9]+ checked, \d+ passed, 1 failed, \d+ not checked
