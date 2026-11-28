// --multi-property decides every check and prints a counterexample for each
// one that fails, the unwound loop included.
// RUN: --no-summarize-array-loops --unwind 2 --multi-property
// EXPECT: (\[COUNTEREXAMPLE\][\s\S]*){3}
// EXPECT: FAILED +\[main\.assertion\.1\] +line 16  unwinding assertion loop 0$
// EXPECT: FAILED +\[main\.assertion\.2\] +line 18
// EXPECT: SUCCESS +\[main\.assertion\.3\] +line 19
// EXPECT: FAILED +\[main\.assertion\.4\] +line 20
// EXPECT: Properties: 4 checked, \d+ passed, 3 failed, \d+ not checked
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
