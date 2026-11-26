// Symbolic exits let later outer iterations overwrite the encoder's bindings.
// Resumption must restore the interrupted iteration's i and precomputed value.
// RUN: --no-summarize-array-loops --incremental-bmc --max-k-step 8
// RUN: --no-summarize-array-loops --incremental-bmc --max-k-step 8 --incremental-solver
// EXPECT: \[PROGRESS\] Resuming main
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

int main(void) {
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n < 6);
  unsigned count = 0;
  for (unsigned i = 0; i < 3; ++i) {
    unsigned value = 2 * i + 1;
    for (unsigned j = 0; j < n; ++j)
      count += value;
    __VERIFIER_assert(count == n * (i + 1) * (i + 1));
  }
  __VERIFIER_assert(count == 9 * n);
}
