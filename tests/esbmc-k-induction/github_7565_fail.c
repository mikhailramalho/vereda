#include <assert.h>

int main()
{
  int i = 0;
  int x = 0;
  goto test;
body:
  i = i + 1;
  if (i == 5)
    x = 1;
  assert(x == 0);
test:
  if (i < 10)
    goto body;
  return 0;
}
// Imported from ESBMC regression/k-induction/github_7565_fail
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^.*Bug found.*$
// Vereda: verdict none
