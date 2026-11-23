#include <assert.h>

int main()
{
  int resp, a, b;

  resp=a-b;

  assert(resp==a-b);
}
// Imported from ESBMC regression/esbmc/overflow_minus2
// RUN: --overflow-check --solver z3
// EXPECT: ^VERIFICATION FAILED$
