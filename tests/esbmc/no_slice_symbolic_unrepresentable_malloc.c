#include <stdlib.h>
#include <assert.h>

size_t __VERIFIER_nondet_size(void);

int main()
{
  // Same defect reached through a symbolic size: the address-space model
  // cannot lay the object out, so the path is vacuously infeasible and the
  // reachable assert(0) is missed.
  size_t n = __VERIFIER_nondet_size();
  __VERIFIER_assume(n >= 0xFFFFFFFFFFFFFFF0UL);
  void *b = malloc(n);
  assert(0);
}
// Imported from ESBMC regression/esbmc/no_slice_symbolic_unrepresentable_malloc
// EXPECT: ^VERIFICATION FAILED$
