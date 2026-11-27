#include <assert.h>
#include <stdatomic.h>

/* Negative counterpart of github_2174: a body-less __c11_atomic_load would
 * return nondet and could satisfy this assertion. */
int main(void)
{
  atomic_int a;

  atomic_init(&a, 0);
  atomic_store(&a, 42);
  assert(atomic_load(&a) == 7);

  return 0;
}
// Imported from ESBMC regression/esbmc/github_2174_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
