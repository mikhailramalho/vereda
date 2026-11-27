// Indices past the end alias real elements in the solver's narrowed index
// sort; defining them made the formula unsatisfiable and hid this failure.
#define N 100
int a[N];
int main()
{
  for (int i = 0; i < N; i++)
    a[i] = i;
  for (int x = 0; x < N; x++)
    __VERIFIER_assert(a[x] != 77); // a[x] != 77
}
// Imported from ESBMC regression/esbmc/array_loop_summary_no_slice_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
