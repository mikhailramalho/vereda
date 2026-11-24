// --multi-property decides every check and prints a counterexample for each
// one that fails, the unwound loop included.
// RUN: --no-summarize-array-loops --unwind 2 --multi-property
// EXPECT: (\[Counterexample\][\s\S]*){3}
// EXPECT: FAILED +\[main\.unwind\.0\]
// EXPECT: FAILED +\[main\.assertion\.1\]
// EXPECT: SUCCESS +\[main\.assertion\.2\]
// EXPECT: FAILED +\[main\.assertion\.3\]
// EXPECT: 3 of 4 properties failed
// EXPECT: VERIFICATION FAILED
extern void __VERIFIER_assert(int);
extern int nondet(void);
int main(void) {
  int x = nondet();
  int n = nondet(), s = 0;
  for (int i = 0; i < n; i++)
    s++;
  __VERIFIER_assert(x != 1);
  __VERIFIER_assert(s < 3);
  __VERIFIER_assert(x != 2);
  return 0;
}
