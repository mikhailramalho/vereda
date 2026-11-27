#include <assert.h>

int main()
{
  int resp, a, b;

  resp=a-b;

  assert(resp==a-b);
}
// Imported from ESBMC regression/esbmc/overflow_minus2
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --solver z3 --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
