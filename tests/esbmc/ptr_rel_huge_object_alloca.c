/* The alloca half of R38: an object above PTRDIFF_MAX would put upper offsets
   in the below-base window of the pointer comparator. alloca has no failure
   outcome, so the bound is assumed rather than branched on. The width is
   fixed: `unsigned long` is 32-bit on LLP64 and never reaches 2^63.
   Spelt __builtin_alloca because Windows ships no <alloca.h>. */
#include <assert.h>
#include <stdint.h>

uint64_t __VERIFIER_nondet_uint64(void);

int main(void)
{
  uint64_t n = __VERIFIER_nondet_uint64();

  char *p = __builtin_alloca(n);
  if (!p)
    return 0;

  char *q = p + n;

  assert(q >= p);
  return 0;
}
// Imported from ESBMC regression/esbmc/ptr_rel_huge_object_alloca
// RUN: --vla-size-check=false --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
