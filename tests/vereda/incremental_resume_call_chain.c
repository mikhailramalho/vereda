// Return through multiple suspended callers and continue within their blocks.
// RUN: --no-summarize-array-loops --incremental-bmc --unlimited-k-steps
// RUN: --no-summarize-array-loops --incremental-bmc --unlimited-k-steps --incremental-solver
// RUN: --no-summarize-array-loops --incremental-bmc --unlimited-k-steps -DSIZE=2000
// EXPECT: \[PROGRESS\] Resuming main
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
#ifndef SIZE
#define SIZE 12
#endif

static int inner(int step) {
  int result = 0;
  for (int i = 0; i < SIZE; i += step)
    ++result;
  return result;
}

static int middle(int x, int step) {
  return x + inner(step) + x * 2;
}

int main(void) {
  int step = __VERIFIER_nondet_int();
  __VERIFIER_assume(step == 1);
  int sum = 0;
  for (int i = 0; i < 3; ++i)
    sum += middle(i, step) + i;
  __VERIFIER_assert(sum == 3 * SIZE + 12);
}
