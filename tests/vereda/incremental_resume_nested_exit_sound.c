// Soundness: an outer loop whose body truncated an inner loop at bound k must
// NOT be checkpointed, otherwise later bounds would reuse truncated inner-loop
// exits from iteration 0 and miss bugs requiring more inner iterations.
// RUN: --no-summarize-array-loops --incremental-bmc --max-k-step 8
// EXPECT-NOT: \[PROGRESS\] Resuming
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
