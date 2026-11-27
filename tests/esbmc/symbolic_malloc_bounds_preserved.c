#include <stdlib.h>

size_t __VERIFIER_nondet_size(void);

int main()
{
  // Bounding a symbolic request to the address space must not weaken the
  // bounds check on a request that fits: b[n] is still out of range.
  size_t n = __VERIFIER_nondet_size();
  __VERIFIER_assume(n >= 10 && n <= 100);
  char *b = malloc(n);
  if (b)
    b[n] = 1;
  return 0;
}
// Imported from ESBMC regression/esbmc/symbolic_malloc_bounds_preserved
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
