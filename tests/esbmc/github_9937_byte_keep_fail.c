// #9937: after a byte write into another member, the out-of-bounds pointer in
// the union still compares equal to itself; the claim below must fail.
int main()
{
  union { int *p; struct { int *a[1]; char c[2]; } s; } u;
  int arr[2];
  u.p = arr + 100;
  u.s.c[1] = 4;
  __VERIFIER_assert(u.p != arr + 100); // pointer kept
}
// Imported from ESBMC regression/esbmc/github_9937_byte_keep_fail
// RUN: -m64
// EXPECT: ^VERIFICATION FAILED$
