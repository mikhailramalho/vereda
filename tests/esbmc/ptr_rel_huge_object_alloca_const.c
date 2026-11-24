/* R39: the PTRDIFF_MAX cap reaches alloca only on the symbolic arm, so a
   constant request above it still laid out an object whose upper offsets are
   the below-base window of the pointer comparator -- R38's witness verbatim.
   A constant cannot be bounded by assumption: the assumption is false and
   proves the program vacuously, so the request is reported instead. */
#include <assert.h>
#include <stdint.h>

int main(void)
{
  const uint64_t n = 0x8000000000000000UL; /* PTRDIFF_MAX + 1 */

  char *p = __builtin_alloca(n);
  if (!p)
    return 0;

  char *q = p + n;

  assert(q >= p);
  return 0;
}
// Imported from ESBMC regression/esbmc/ptr_rel_huge_object_alloca_const
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: alloca: size exceeds PTRDIFF_MAX
// Vereda: unmet EXPECT: alloca: size exceeds PTRDIFF_MAX
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
