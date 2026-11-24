// Loops a summary would get wrong are unrolled: one reads an element
// another iteration wrote, one writes two arrays, and in the last an
// assumption ends every path part way, so the assertion is unreachable.
// RUN: --unwind 11
// EXPECT-NOT: Summarised
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
int a[10], b[10];
int main(void) {
  a[0] = 1;
  for (int i = 1; i < 10; i++)
    a[i] = a[i - 1] + 1;
  __VERIFIER_assert(a[9] == 10);
  for (int i = 0; i < 10; i++) {
    a[i] = 0;
    b[i] = 0;
  }
  __VERIFIER_assert(a[3] == 0 && b[3] == 0);
  for (int i = 0; i < 10; i++) {
    __VERIFIER_assume(i < 5);
    a[i] = 1;
  }
  __VERIFIER_assert(a[7] == 0);
  return 0;
}
