// Resume the first truncated inner loop, rather than an outer prefix containing
// incomplete inner iterations, to find the bug requiring all 16 increments.
// RUN: --no-summarize-array-loops --incremental-bmc --max-k-step 8
// EXPECT: \[PROGRESS\] Resuming main
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

int main(void) {
  int count = 0;
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      if (__VERIFIER_nondet_int())
        break;
      ++count;
    }
  }
  __VERIFIER_assert(count != 16);
  return 0;
}
