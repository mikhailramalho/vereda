// Each element gets its own nondet value.
int __VERIFIER_nondet_int();
#define N 100000
int a[N], b[N];
int main()
{
  for (int i = 0; i < N; i++)
    a[i] = __VERIFIER_nondet_int();
  for (int i = 0; i < N; i++)
    b[i] = a[i];
  for (int x = 0; x < N; x++)
    __VERIFIER_assert(b[x] == a[x]); // copied
}
// Imported from ESBMC regression/esbmc/array_loop_summary_nondet
// Needs: --summarize-array-loops
// RUN: --summarize-array-loops
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
