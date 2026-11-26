// A bug in the post-loop suffix first reachable after resuming a symbolic-exit
// loop must be reported with its stable assertion id (not drifted across bounds).
// RUN: --no-summarize-array-loops --incremental-bmc --max-k-step 8
// EXPECT: \[PROGRESS\] Resuming main
// EXPECT: FAILED\s+\[main\.assertion\.1\]
// EXPECT: ^VERIFICATION FAILED$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

int main(void) {
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n < 7);
  unsigned count = 0;
  while (count < n)
    ++count;
  __VERIFIER_assert(count != 5);
  return 0;
}
