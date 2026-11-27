/* #10891: whole-struct read from a scalar array at a symbolic offset. */
#include <assert.h>
#include <stdint.h>
unsigned __VERIFIER_nondet_uint(void);
struct S { uint32_t v; };
int main(void)
{
  uint32_t buf[2] = {7, 7};
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 2);
  const struct S *p = (const struct S *)&buf[i];
  struct S s = *p;
  assert(s.v == 7);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10891
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
