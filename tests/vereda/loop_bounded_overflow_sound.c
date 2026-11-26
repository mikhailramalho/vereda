// RUN: --incremental-bmc --k-step 2 --unlimited-k-steps
// EXPECT: ^VERIFICATION FAILED$
// Without nsw, an overflowing signed step-2 counter has no finite SCEV max
// backedge-taken count and must not be claimed bounded (#253).
extern void __VERIFIER_assert(int);
int main(void) {
  int s = 0;
  for (int i = 0; i < 2147483647; i += 2) {
    s++;
    if (s > 10)
      __VERIFIER_assert(0);
  }
  return 0;
}
