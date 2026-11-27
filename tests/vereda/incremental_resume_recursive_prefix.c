// Reuse work completed in each recursive frame, then restore its result on return.
// RUN: --no-summarize-array-loops --incremental-bmc --unlimited-k-steps
// RUN: --no-summarize-array-loops --incremental-bmc --unlimited-k-steps --incremental-solver
// RUN: --no-summarize-array-loops --incremental-bmc --unlimited-k-steps -DSIZE=200 -DWORK=128
// EXPECT: \[PROGRESS\] Resuming main
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
#ifndef SIZE
#define SIZE 6
#endif
#ifndef WORK
#define WORK 8
#endif

static unsigned sum(unsigned n) {
  unsigned work = 0;
  for (unsigned i = 0; i < WORK; ++i)
    work += i;
  if (n == 0)
    return 0;
  return n + sum(n - 1) + work;
}

int main(void) {
  unsigned result = sum(SIZE);
  __VERIFIER_assert(result == SIZE * (SIZE + 1) / 2 + SIZE * WORK * (WORK - 1) / 2);
}
