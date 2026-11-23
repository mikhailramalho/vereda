// #9937: a pointer read from the union in a later step than the store keeps
// its provenance.
int main()
{
  union { int *p; int a[2]; } u;
  int arr[2];
  u.p = arr + 100;
  u.a[0] = u.a[0];
  int *q = u.p;
  __VERIFIER_assert(q != arr + 100); // q kept
}
// Imported from ESBMC regression/esbmc/github_9937_later_step_fail
// RUN: -m64
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
