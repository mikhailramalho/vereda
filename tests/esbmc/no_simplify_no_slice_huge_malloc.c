#include <stdlib.h>
#include <assert.h>

int main()
{
  // A negative size widens to a huge size_t. Under --no-simplify --no-slice
  // this makes every later assertion vacuously unreachable, so the reachable
  // assert(0) below is missed and ESBMC reports SUCCESSFUL.
  void *b = malloc(-4);
  assert(0);
}
// Imported from ESBMC regression/esbmc/no_simplify_no_slice_huge_malloc
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
