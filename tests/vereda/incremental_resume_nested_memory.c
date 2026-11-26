// Suffix rollback restores path-local memory for the interrupted inner loop.
// RUN: --no-summarize-array-loops --incremental-bmc --max-k-step 8
// RUN: --no-summarize-array-loops --incremental-bmc --max-k-step 8 --ub-uninit-check
// RUN: --no-summarize-array-loops --incremental-bmc --max-k-step 8 -m32
// EXPECT: \[PROGRESS\] Resuming main
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

int main(void) {
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n < 6);
  unsigned a[3] = {0, 0, 0};
  for (unsigned i = 0; i < 3; ++i) {
    for (unsigned j = 0; j < n; ++j)
      a[i] += i + 1;
    __VERIFIER_assert(a[i] == (i + 1) * n);
  }
  __VERIFIER_assert(a[0] + a[1] + a[2] == 6 * n);
}
