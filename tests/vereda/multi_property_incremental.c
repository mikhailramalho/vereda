// Under --incremental-bmc, --multi-property moves to the next bound while
// only loops fail, then shows a counterexample for each failed assertion.
// RUN: --no-summarize-array-loops --incremental-bmc --multi-property
// EXPECT: Bound 2 too small
// EXPECT: (\[COUNTEREXAMPLE\][\s\S]*){2}
// EXPECT-NOT: (\[COUNTEREXAMPLE\][\s\S]*){3}|Bound 3 too small
// EXPECT: Properties: 2 checked, \d+ passed, 2 failed, \d+ not checked
// EXPECT: VERIFICATION FAILED
extern void __VERIFIER_assert(int);
extern int nondet(void);
int main(void) {
  int x = nondet();
  for (int i = 0; i < 2; i++)
    x++;
  __VERIFIER_assert(x != 3);
  __VERIFIER_assert(x != 4);
  return 0;
}
