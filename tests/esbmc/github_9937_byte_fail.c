/* #9937: a byte write next to an out-of-bounds pointer in a union. */
int main()
{
  union { int *p; struct { int *a[1]; char c[2]; } s; } u;
  int arr[2];
  u.p = arr + 100;
  u.s.c[1] = 4;
  __VERIFIER_assert(0); // show
}
// Imported from ESBMC regression/esbmc/github_9937_byte_fail
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \[main.assertion.1\]  line 8  show
// Vereda: verdict FAILED
