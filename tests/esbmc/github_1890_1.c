#include <assert.h>
int main()
{
  int num = 0;
  int a = 5 / num; // division by zero in line 3

  for (int i = 0; i <= 10; i++)
  {
    assert(1);
  }

  int num1 = 0;
  int b = 5 / num1; // division by zero in line 7
}
// Imported from ESBMC regression/esbmc/github_1890_1
// Needs: --k-induction
// RUN: --multi-property --overflow-check --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \bdivision by zero\b
// EXPECT: \bnum != 0\b
// EXPECT: \bdivision by zero\b
// EXPECT: \bnum1 != 0\b
// Vereda: verdict none
