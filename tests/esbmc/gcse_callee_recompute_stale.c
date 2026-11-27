#include <assert.h>

unsigned __VERIFIER_nondet_uint();
unsigned a, b, sink;

void f()
{
  a = a + 1u;
  sink = a + b;
}

// f writes `a` and recomputes `a + b`, which the analysis carries back to the
// caller without assigning the caller's symbol for it (#7992).
int main()
{
  a = __VERIFIER_nondet_uint();
  b = __VERIFIER_nondet_uint();
  unsigned x = a + b;
  f();
  unsigned z = a + b;
  assert(z == x + 1u);
}
// Imported from ESBMC regression/esbmc/gcse_callee_recompute_stale
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
