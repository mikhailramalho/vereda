#include <assert.h>
#include <stdlib.h>

/* Each assumption below is false under the real memory model, so main is
   unreachable. Were the __CPROVER_* primitives havoc'd, every assumption would
   be satisfiable and the assert(0) reachable. */
int main()
{
  char a[4];
  char *d = malloc(8);
  __VERIFIER_assume(__CPROVER_r_ok(a, 8));
  __VERIFIER_assume(__CPROVER_w_ok(a + 3, 2));
  __VERIFIER_assume(__CPROVER_OBJECT_SIZE(a) != 4);
  __VERIFIER_assume(__CPROVER_DYNAMIC_OBJECT(a));
  __VERIFIER_assume(!__CPROVER_same_object(a, a + 1));
  __VERIFIER_assume(!__CPROVER_DYNAMIC_OBJECT(d));
  assert(0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_2457-assume
// Needs: __CPROVER_DYNAMIC_OBJECT, __CPROVER_OBJECT_SIZE, __CPROVER_r_ok, __CPROVER_same_object, __CPROVER_w_ok
// RUN: --force-malloc-success --ub-pointer-check=false --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
