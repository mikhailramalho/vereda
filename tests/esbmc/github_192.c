int linear_search(int *a, int n, int q)
{
  unsigned int j = 0;
  while (j < n && a[j] != q)
    j++;

  if (j < n)
    return 1;
  else
    return 0;
}

int main()
{
  unsigned int SIZE = __VERIFIER_nondet_uint() / 2 + 1;
  int a[SIZE];
  a[SIZE / 2] = 3;
  assert(linear_search(a, SIZE, 3));
}
// Imported from ESBMC regression/esbmc/github_192
// Needs: REQUIRES arch32
// RUN: --32 -Wno-error=implicit-function-declaration --no-unwinding-assertions --unwind 1
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
