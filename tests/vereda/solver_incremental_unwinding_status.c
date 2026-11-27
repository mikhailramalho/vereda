// An unwinding assertion failing under --incremental-solver leaves the
// division-by-zero check, which held in the first phase, checked.
// RUN: --incremental-solver --unwind 2
// EXPECT: FAILED +\[main\.unwind\.0\]
// EXPECT: SUCCESS +\[main\.division-by-zero\.1\]
// EXPECT: ^Properties: 2 checked, 1 passed, 1 failed, 0 not checked$
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int n = __VERIFIER_nondet_int();
  int s = 0;
  for (int k = 0; k < n; ++k)
    s = k;
  int d = __VERIFIER_nondet_int();
  if (d * d > 3)
    s = 100 / d;
  return s;
}
