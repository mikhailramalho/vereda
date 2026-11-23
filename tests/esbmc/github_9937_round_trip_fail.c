// #9937: reading an out-of-bounds pointer through another union member and
// storing it back made the path vacuous.
int main()
{
  union { int *p; int *q; } u;
  int arr[2];
  u.p = arr + 100;
  u.q = u.q;
  __VERIFIER_assert(u.p != arr + 100); // round trip
}
// Imported from ESBMC regression/esbmc/github_9937_round_trip_fail
// RUN: -m64
// EXPECT: ^VERIFICATION FAILED$
