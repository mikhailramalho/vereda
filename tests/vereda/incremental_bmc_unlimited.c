// --unlimited-k-steps raises the bound past --max-k-step's 50 until the
// verdict no longer depends on it: the bug here needs 120 iterations.
// --k-step 2 checks every other bound and still gets there (#84). The
// walks print no line per iteration: at each bound they repeat every
// iteration, and an unbounded run wrote gigabytes of them.
// RUN: --no-summarize-array-loops --incremental-bmc --unlimited-k-steps --k-step 2
// EXPECT: Checking bound k = 121$
// EXPECT-NOT: Checking bound k = 2$
// EXPECT-NOT: Unwinding loop
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
int main(void) {
  int i = 0;
  while (i < 120)
    i++;
  __VERIFIER_assert(i != 120);
  return 0;
}
