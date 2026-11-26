// Early returns and sibling paths are retained while an inner loop is resumed.
// RUN: --no-summarize-array-loops --incremental-bmc --max-k-step 8
// EXPECT: \[PROGRESS\] Resuming main
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

int main(void) {
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n < 6);
  unsigned count = 0;
  if (__VERIFIER_nondet_uint()) {
    for (unsigned i = 0; i < 3; ++i) {
      for (unsigned j = 0; j < n; ++j) {
        if (i == 1 && j == 3) {
          __VERIFIER_assert(count == n + 3);
          return 0;
        }
        ++count;
      }
      __VERIFIER_assert(count == (i + 1) * n);
    }
    __VERIFIER_assert(count == 3 * n);
  } else {
    __VERIFIER_assert(count == 0);
  }
}
