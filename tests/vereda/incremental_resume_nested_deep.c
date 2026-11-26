// Resume an inner loop and finish all enclosing iterations and their suffixes.
// The symbolic step prevents SCEV from skipping the intermediate bounds.
// RUN: --no-summarize-array-loops --incremental-bmc --unlimited-k-steps
// RUN: --no-summarize-array-loops --incremental-bmc --unlimited-k-steps --incremental-solver
// RUN: --no-summarize-array-loops --incremental-bmc --unlimited-k-steps -DSIZE=2000
// EXPECT: \[PROGRESS\] Resuming main
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

#ifndef SIZE
#define SIZE 20
#endif

int main(void) {
  int step = __VERIFIER_nondet_int();
  __VERIFIER_assume(step == 1);
  int count = 0, suffix = 0;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 2; ++j) {
      for (int k = 0; k < SIZE; k += step)
        ++count;
      suffix += i + j;
    }
    suffix += 10 * i;
  }
  __VERIFIER_assert(count == 6 * SIZE);
  __VERIFIER_assert(suffix == 39);
}
