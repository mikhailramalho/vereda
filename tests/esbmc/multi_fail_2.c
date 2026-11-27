#include <assert.h>

int main()
{
  int arr[1];
  arr[3] = 10;
  for (int i = 0; i < 10; i++)
  {
    assert(1 == 0);
  }
}
// Imported from ESBMC regression/esbmc/multi_fail_2
// Needs: --multi-fail-fast 1
// RUN: --multi-property --multi-fail-fast 1 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
