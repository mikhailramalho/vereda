// A recursive call within a loop must resume the rest of the block, the next
// loop iteration and every suspended recursive caller in order.
// RUN: --no-summarize-array-loops --incremental-bmc --max-k-step 8
// RUN: --no-summarize-array-loops --incremental-bmc --max-k-step 8 --incremental-solver
// EXPECT: \[PROGRESS\] Resuming main
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);

static unsigned tree(unsigned n) {
  if (n == 0)
    return 1;
  unsigned result = 0;
  for (unsigned i = 0; i < 2; ++i)
    result += tree(n - 1) + n + i;
  return result;
}

int main(void) {
  __VERIFIER_assert(tree(3) == 37);
}
