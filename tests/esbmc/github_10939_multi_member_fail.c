/* As github_10939_multi_member, but asserting the float write is lost. */
#include <assert.h>
#include <stddef.h>
struct d { unsigned c; };
union u { unsigned char b[4]; unsigned w; float f; };
size_t __VERIFIER_nondet_size(void);
int main(void)
{
  struct d d = {0};
  size_t off = __VERIFIER_nondet_size();
  __VERIFIER_assume(off == 0);
  union u *p = (union u *)((unsigned char *)&d + off);
  union u n;
  n.f = 1.0f;
  *p = n;
  assert(d.c == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10939_multi_member_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^Properties: [0-9]+ checked, \d+ passed, 1 failed, \d+ not checked
