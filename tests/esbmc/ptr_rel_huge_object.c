/* An offset at or above 2^63 would read negative under the signed comparison,
   so one-past-the-end would sort below the base. Allocation is capped at
   PTRDIFF_MAX, which puts every defined offset below that line. The width is
   fixed: `unsigned long` is 32-bit on LLP64 and never reaches 2^63. */
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t __VERIFIER_nondet_uint64(void);

int main(void)
{
  uint64_t n = __VERIFIER_nondet_uint64();

  char *p = malloc(n);
  if (!p)
    return 0;

  char *q = p + n;

  assert(q >= p);
  return 0;
}
// Imported from ESBMC regression/esbmc/ptr_rel_huge_object
// EXPECT: ^VERIFICATION SUCCESSFUL$
