// --incremental-solver keeps the incremental assumption-based solver path.
// RUN: --incremental-solver --multi-property --no-summarize-array-loops --unwind 2
// EXPECT: FAILED +\[main\.assertion\.1\]
// EXPECT: SUCCESS +\[main\.assertion\.2\]
// EXPECT: FAILED +\[main\.assertion\.3\] +line 14
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
int main(void) {
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assert(x > 0);
  __VERIFIER_assert(x == x);
  int n = __VERIFIER_nondet_int();
  for (int i = 0; i < n; ++i)
    ;
}
