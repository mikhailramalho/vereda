#include <assert.h>

int main()
{
  int i = 0;
  goto test;
body:
  assert(i < 10);
  i = i + 1;
test:
  if (i < 10)
    goto body;
  return 0;
}
// Imported from ESBMC regression/k-induction/github_7565
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^Solution found by the inductive step.*$
// Vereda: verdict none
