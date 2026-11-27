#include <string.h>
#include <assert.h>

typedef _Bool bool;

int main()
{
  bool v = __VERIFIER_nondet_bool();
  memset(&v, 0, sizeof(v));
  assert(!v);
}

// Imported from ESBMC regression/esbmc/github_1458
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
