#include <assert.h>

int main()
{
  unsigned int resp, a, b;

  resp=a-b;

  assert(resp==a-b);
}
// Imported from ESBMC regression/esbmc/overflow_minus3
// RUN: --unsigned-overflow-check --solver z3
// EXPECT: ^VERIFICATION FAILED$
