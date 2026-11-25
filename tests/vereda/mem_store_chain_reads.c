// Exceed the known-byte cache so first reads must traverse flushed stores.
// RUN: --no-summarize-array-loops
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
#define N 270000
unsigned a[N];
volatile unsigned sink;
int main(void) {
  for (unsigned i = 0; i < N; ++i)
    a[i] = 3 * i;
  for (unsigned i = 0; i < N; ++i)
    sink = a[i];
  __VERIFIER_assert(sink == 3 * (N - 1));
}
