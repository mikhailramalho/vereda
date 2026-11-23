#include <assert.h>

void f(float beta, int n)
{
  float s = 0.0f;
  for (int i = 0; i < n; ++i)
    s += 1.0f;
  assert(beta == 0.0f);
}

int main()
{
  f(0.0f, 1000);
  return 0;
}
// Imported from ESBMC regression/k-induction/github_10240_proved
// Needs: --interval-analysis --k-induction
// RUN: --interval-analysis --k-induction --max-k-step 3
// MODE: FUTURE
// EXPECT: ^Solution found by the inductive step
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
