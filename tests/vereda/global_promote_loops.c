// A global loop counter that only main uses becomes a local of main, so the
// three array loops over it are summarised (#380; SV-COMP's
// array-lopstr16/partial_lesser_bound-1 has the same loops).
// EXPECT: ^\[INFO\] Summarised 3 array loop\(s\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
int a[1000];
int i;
int main(void) {
  for (i = 0; i < 1000; i++)
    a[i] = 0;
  for (i = 0; i < 500; i++)
    a[i] = 10;
  for (i = 0; i < 500; i++)
    __VERIFIER_assert(a[i] == 10);
  return 0;
}
