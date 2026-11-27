// #9937: a write to one struct member inside a union keeps the pointer that
// the other member's bits hold.
int main()
{
  union { int *p; struct { long x; int *b; } s; } u;
  int arr[2];
  u.p = arr + 100;
  u.s.b = 0;
  __VERIFIER_assert(u.p == arr + 100); // p kept
  __VERIFIER_assert(u.s.b == 0); // b written
}
// Imported from ESBMC regression/esbmc/github_9937_nested
// RUN: -m64
// EXPECT: ^VERIFICATION SUCCESSFUL$
