// --summarize-array-loops also replaces a loop that only checks; the
// element written between the loops fails its check.
// RUN: --summarize-array-loops
// EXPECT: ^Summarised 2 array loop\(s\)$
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
#define N 100000
int a[N];
int main(void) {
  for (int i = 0; i < N; i++)
    a[i] = 5;
  a[12345] = 6;
  for (int i = 0; i < N; i++)
    __VERIFIER_assert(a[i] == 5);
  return 0;
}
