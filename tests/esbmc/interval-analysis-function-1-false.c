#include <assert.h>

int foo(int n)
{
  // n: [1,40]
  assert(n == 42); // always false
}


int main()
{
  foo(1);
  foo(40);
}
// Imported from ESBMC regression/esbmc/interval-analysis-function-1-false
// Needs: --interval-analysis
// RUN: --interval-analysis --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
