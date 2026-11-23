void __VERIFIER_assert(int cond) { __VERIFIER_assert(cond); } // cond
#define N 100000
int a[N], b[N];
int main()
{
  for (int i = 0; i < N; i++)
    a[i] = 42;
  for (int i = 0; i < N - 1; i++)
    b[i] = a[i];
  for (int x = 0; x < N; x++)
    __VERIFIER_assert(b[x] == 42);
}
// Imported from ESBMC regression/esbmc/array_loop_summary_k_induction_fail
// Needs: --summarize-array-loops --k-induction
// RUN: --summarize-array-loops --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
