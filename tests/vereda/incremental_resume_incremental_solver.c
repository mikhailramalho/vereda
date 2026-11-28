// --incremental-solver keeps one solver instance across the resumed bounds,
// asking each check under an assumption, and still finds a bug past bound 1.
// RUN: --incremental-solver --no-summarize-array-loops --incremental-bmc --max-k-step 10
// EXPECT: \[PROGRESS\] Resuming main
// EXPECT: FAILED +\[main\.assertion\.2\]
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
int i, a[8];
int main(void) {
  for (i = 0; i < 8; ++i) {
    a[i] = __VERIFIER_nondet_int();
    __VERIFIER_assert(i < 5 || a[i] != 3);
  }
}
