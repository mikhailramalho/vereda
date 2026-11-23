#include <assert.h>

int main()
{
  int status = 0;

  int cnt = 4;
  while(cnt--)
  {
  }
  do

  {
    assert(0);

  } while(0);
}
// Imported from ESBMC regression/k-induction/github_1092_6_false
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
