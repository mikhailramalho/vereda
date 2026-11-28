// The results list shows the checks the solver decided. Assertions are
// decided first, so here the loop's check, which comes first in the walk,
// is never decided and must not be listed; it still takes the first
// number, as in ESBMC 8.8.
// RUN: --no-summarize-array-loops --unwind 3
// EXPECT: FAILED +\[main\.assertion\.2\] +line 15
// EXPECT: Properties: 1 checked, \d+ passed, 1 failed, 1 not checked
// EXPECT-NOT: unwinding assertion
extern void __VERIFIER_assert(int);
extern int nondet(void);
int main(void) {
  int n = nondet(), s = 0;
  for (int i = 0; i < n; i++)
    s++;
  __VERIFIER_assert(s != 1);
  return 0;
}
