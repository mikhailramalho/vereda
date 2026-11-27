#include <assert.h>

/* is_gcc_polymorphic_builtin matches on a name *prefix*, so an ordinary user
 * function can select an arm that then binds arguments.front(). Called with no
 * arguments there is no front() to bind. */
int __sync_fetch_and_add_mine(void);

int main(void)
{
  int r = __sync_fetch_and_add_mine();
  assert(r == r);
  return 0;
}
// Imported from ESBMC regression/esbmc/polymorphic_builtin_prefix_no_args
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
