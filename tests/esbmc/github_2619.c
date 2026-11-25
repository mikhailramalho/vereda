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
// Imported from ESBMC regression/esbmc/github_2619
// Needs: --parallel-solving --k-induction
// RUN: --parallel-solving --k-induction
// MODE: FUTURE
// EXPECT: ^Properties: 2 checked, \d+ passed, 2 failed, \d+ not checked$
// Vereda: verdict none
