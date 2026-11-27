#include <assert.h>

int main()
{
  unsigned int resp, a, b;

  resp=a-b;

  assert(resp==a-b);
}
// Imported from ESBMC regression/esbmc/overflow_minus3
// RUN: --ub-signed-overflow-check --unsigned-overflow-check --ub-shift-check --solver z3
// EXPECT: ^VERIFICATION FAILED$
