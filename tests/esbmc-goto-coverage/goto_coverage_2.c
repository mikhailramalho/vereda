#include <assert.h>

void test()
{
  int x = 1;
  assert(x == 1);
}

int test1()
{
  return 2;
}

int main()
{
  test();
  int y = test1();
  assert(y++ > 0);
}
// Imported from ESBMC regression/goto-coverage/goto_coverage_2
// Needs: --assertion-coverage
// RUN: --multi-property --assertion-coverage --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
