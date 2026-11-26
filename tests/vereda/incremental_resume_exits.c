// Top-level loops with symbolic exits resume by rolling back the post-loop
// suffix and extending the loop prefix at each bound.
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
  while (count < n)
    ++count;
  __VERIFIER_assert(count == n);
}
