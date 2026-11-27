#include <assert.h>

/* Every arm of the matcher treats arguments.front() as a pointer, but the name
 * prefix it matches on is not reserved to the builtin, so a user function can
 * reach it with a non-pointer first argument. */
int __atomic_load_n_bogus(int);

int main(void)
{
  int r = __atomic_load_n_bogus(3);
  assert(r == r);
  return 0;
}
// Imported from ESBMC regression/esbmc/polymorphic_builtin_prefix_nonptr
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
