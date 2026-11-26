// Symbolic-exit loop with early return, multiple exits, and a shared callee
// asserting in both the loop body and the post-loop suffix: rolling back the
// suffix must restore prefix claim terms and assertion numbering cleanly.
// RUN: --no-summarize-array-loops --incremental-bmc --max-k-step 10
// EXPECT: \[PROGRESS\] Resuming main
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

static void check_nonneg(int v) {
  __VERIFIER_assert(v >= 0);
}

int main(void) {
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n <= 6);
  int acc = 0;
  for (unsigned i = 0; i < n; ++i) {
    acc += 2;
    check_nonneg(acc);
    if (i == 4)
      return 0;
  }
  check_nonneg(acc);
  __VERIFIER_assert(acc <= 8);
  return 0;
}
