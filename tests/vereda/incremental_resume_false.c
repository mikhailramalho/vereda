// New iterations must extend existing claims without retaining the old bound check.
// RUN: --no-summarize-array-loops --incremental-bmc --max-k-step 10
// EXPECT: \[PROGRESS\] Resuming main
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
int i, a[8];
int main(void) {
  for (i = 0; i < 8; ++i) {
    a[i] = __VERIFIER_nondet_int();
    __VERIFIER_assert(i < 5);
  }
}
