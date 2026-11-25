// The results list shows the checks the solver decided. Assertions are
// decided first, so here the loop's check, which comes first in the walk,
// is never decided and must not be listed.
// RUN: --unwind 3
// EXPECT: FAILED +\[main\.assertion\.1\]
// EXPECT: Properties: 1 checked, \d+ passed, 1 failed, \d+ not checked
// EXPECT-NOT: main\.unwind\.0
extern void __VERIFIER_assert(int);
extern int nondet(void);
int main(void) {
  int n = nondet(), s = 0;
  for (int i = 0; i < n; i++)
    s++;
  __VERIFIER_assert(s != 1);
  return 0;
}
