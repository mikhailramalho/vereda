// --summarize-array-loops also replaces a loop that only checks: one
// iteration at a nondet index covers all of them.
// RUN: --summarize-array-loops
// EXPECT: ^Summarised 2 array loop\(s\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
#define N 100000
int a[N];
int main(void) {
  for (int i = 0; i < N; i++)
    a[i] = 5;
  for (int i = 0; i < N; i++)
    __VERIFIER_assert(a[i] == 5);
  return 0;
}
