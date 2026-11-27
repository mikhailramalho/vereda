// #9937: a pointer whose bits span two int elements of a union member is not
// read from one element; storing it back keeps the path reachable.
int main()
{
  union { int *p; int a[2]; } u;
  int arr[2];
  u.p = arr + 100;
  u.a[1] = 7;
  u.p = u.p;
  __VERIFIER_assert(0); // reachable
}
// Imported from ESBMC regression/esbmc/github_9937_int_array_fail
// RUN: -m64 --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
