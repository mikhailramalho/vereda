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
// EXPECT: ^\*\* 2 of 2 properties failed$
// Vereda: verdict none
