int main()
{
  int a[2][3] = {{1, 1, 1}, {1, 1, 1}};
  int sum = 0;
  for (int *p = &a[0][0]; p < &a[1][2]; p++)
    sum += *p;
  __VERIFIER_assert(sum == 5); // flat walk covers five elements
  return 0;
}
// Imported from ESBMC regression/esbmc/nested_index_bound_lt
// RUN: --unwind 8
// EXPECT: ^VERIFICATION SUCCESSFUL$
