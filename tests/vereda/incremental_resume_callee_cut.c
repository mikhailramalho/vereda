// Resume a callee loop, return its value and finish the caller's iteration.
// RUN: --no-summarize-array-loops --incremental-bmc --max-k-step 8
// EXPECT: \[PROGRESS\] Resuming main
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
