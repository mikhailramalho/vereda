#include <assert.h>

void func(int x)
{
  assert(x != 1);
  if (x > 10)
  {
    x = x + 1;
  }
}

int main()
{
  int x = 1;
  func(x);
}
// Imported from ESBMC regression/goto-coverage/cov_assume_asserts_1
// Needs: --branch-coverage --cov-assume-asserts
// RUN: --branch-coverage --cov-assume-asserts --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^Branches : 2
// EXPECT: ^Reached : 0
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
