#include <assert.h>

int foo(int n)
{
  // n: [42,42]
  assert(n == 42); // always holds
}


int main()
{
  foo(42);
}
// Imported from ESBMC regression/esbmc/interval-analysis-function-1
// Needs: --interval-analysis
// RUN: --interval-analysis
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
