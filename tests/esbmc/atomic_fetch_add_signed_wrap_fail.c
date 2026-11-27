#include <limits.h>
#include <stdatomic.h>

atomic_int a = INT_MAX;

int main()
{
  int old = atomic_fetch_add(&a, 1);
  return old + 1;
}
// Imported from ESBMC regression/esbmc/atomic_fetch_add_signed_wrap_fail
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: line 9 column [0-9]+ function main$
// EXPECT: arithmetic overflow on add
