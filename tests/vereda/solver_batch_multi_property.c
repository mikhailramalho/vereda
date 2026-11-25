// Disjunctive check batching under --multi-property: mixes constant-false
// checks, mutually exclusive failing checks (needing multiple SAT rounds),
// duplicate failing conditions, and a safe check.
// RUN: --multi-property
// EXPECT: (\[COUNTEREXAMPLE\][\s\S]*){3}
// EXPECT: SUCCESS +\[main\.assertion\.1\]
// EXPECT: FAILED +\[main\.assertion\.2\]
// EXPECT: FAILED +\[main\.assertion\.3\]
// EXPECT: SUCCESS +\[main\.assertion\.4\]
// EXPECT: FAILED +\[main\.assertion\.5\]
// EXPECT: Properties: 5 checked, 2 passed, 3 failed, 0 not checked
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assert(x == x);          // folds to constant false violation
  __VERIFIER_assert(x != 1);          // fails when x == 1
  __VERIFIER_assert(x != 2);          // fails when x == 2 (mutually exclusive with x == 1)
  __VERIFIER_assert(x - x == 0);      // safe
  __VERIFIER_assert(x != 1);          // duplicate violation term of assertion 2
  return 0;
}
