#include <assert.h>
#include <stddef.h>

// Negative variant of github_5658_object_size_nondet: the modelled size is
// a genuinely unconstrained nondet value (not pinned to 0 or any other
// constant), so asserting it equals a fixed constant must fail.
size_t f(char *p)
{
  size_t n = __ESBMC_get_object_size(p);
  assert(n == 42);
  return n;
}

int main()
{
  return 0;
}
// Imported from ESBMC regression/esbmc/github_5658_object_size_nondet_fail
// Needs: __ESBMC_get_object_size
// RUN: --entry f
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
