// A truncated callee still requires rewalking its caller; no callee continuation
// is saved by the nested-loop checkpoints in the entry function.
// RUN: --no-summarize-array-loops --incremental-bmc --max-k-step 8
// EXPECT-NOT: \[PROGRESS\] Resuming
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);

static int count(int n) {
  int result = 0;
  for (int j = 0; j < n; ++j)
    ++result;
  return result;
}

int main(void) {
  int sum = 0;
  for (int i = 0; i < 3; ++i)
    sum += count(5);
  __VERIFIER_assert(sum != 15);
}
