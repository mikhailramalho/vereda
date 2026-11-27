// #9937: the byte write leaves the out-of-bounds pointer in the union intact.
int main()
{
  union { int *p; struct { int *a[1]; char c[2]; } s; } u;
  int arr[2];
  u.p = arr + 100;
  u.s.c[1] = 4;
  __VERIFIER_assert(u.p == arr + 100); // pointer kept
  __VERIFIER_assert(u.s.a[0] == arr + 100); // pointer read through the struct
  __VERIFIER_assert(u.s.c[1] == 4); // byte written
}
// Imported from ESBMC regression/esbmc/github_9937_byte_keep
// RUN: -m64 --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
