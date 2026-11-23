#include <assert.h>

// Interval analysis gives beta the singleton [0, 0]. It used to assume
// beta == 1.4e-45f there, which no run satisfies, so the inductive step
// proved the false assertion below.
void f(float beta, int n)
{
  float s = 0.0f;
  for (int i = 0; i < n; ++i)
    s += 1.0f;
  assert(beta != 0.0f);
}

int main()
{
  f(0.0f, 1000);
  return 0;
}
// Imported from ESBMC regression/k-induction/github_10240
// Needs: --interval-analysis --k-induction
// RUN: --interval-analysis --k-induction --max-k-step 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none
