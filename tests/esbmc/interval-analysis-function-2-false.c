#include <assert.h>
int foo() {
  return 22;
}

int main()
{
  int A = foo();
  assert(A == 42);
  A = foo();
  assert(A > 40);
}


// Imported from ESBMC regression/esbmc/interval-analysis-function-2-false
// Needs: --interval-analysis
// RUN: --interval-analysis
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
