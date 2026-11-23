// #10154: only the head test decides whether the loop is skipped. The exit
// test in the body reads what the body wrote, so a state that would fail it
// before the loop still enters the loop.
#include <assert.h>

int main(void)
{
  int i = 5, n = 1000;
  while (i < n)
  {
    i++;
    if (i == 5)
      break;
  }
  assert(i != 1000);
  return 0;
}
// Imported from ESBMC regression/k-induction/github_10154_body_exit
// Needs: --k-induction
// RUN: --k-induction --max-k-step 10
// MODE: FUTURE
// EXPECT: ^VERIFICATION (UNKNOWN|FAILED)$
// Vereda: verdict none
