// Constant-false checks are marked Holds without invoking the solver.
// RUN: --multi-property
// EXPECT: SUCCESS +\[main\.assertion\.1\]
// EXPECT: SUCCESS +\[main\.assertion\.2\]
// EXPECT: SUCCESS +\[main\.assertion\.3\]
// EXPECT: SUCCESS +\[main\.assertion\.4\]
// EXPECT: \*\* 0 of 4 properties failed
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
int main(void) {
  int a = 3, b = 4;
  __VERIFIER_assert(a + b == 7);
  __VERIFIER_assert(b - a == 1);
  __VERIFIER_assert(a * b == 12);
  __VERIFIER_assert(a < b);
  return 0;
}
