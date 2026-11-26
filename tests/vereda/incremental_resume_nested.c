// Resume an inner loop without replaying its prefix nondeterministic calls.
// RUN: --no-summarize-array-loops --incremental-bmc --max-k-step 8
// EXPECT: \[PROGRESS\] Resuming
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
int main(void) {
  int count = 0;
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 6; ++j) {
      __VERIFIER_nondet_int();
      ++count;
    }
  __VERIFIER_assert(count == 18);
}
