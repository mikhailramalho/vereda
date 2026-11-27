#include <assert.h>
int foo() {
  return 42;
}

int main()
{
  int A = foo();
  assert(A == 42);
  A = foo();
  assert(A > 40);
}


// Imported from ESBMC regression/esbmc/interval-analysis-function-2
// Needs: --interval-analysis
// RUN: --interval-analysis --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
