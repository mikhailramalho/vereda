// Do not extend toward a distant finite bound before asking whether the
// cut is feasible, even under unlimited incremental BMC.
// RUN: --no-summarize-array-loops --unlimited-k-steps
// EXPECT-NOT: Deferring unwinding-only solve
// EXPECT-NOT: Checking bound k = 2$
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern unsigned __VERIFIER_nondet_uint(void);
int main(void) {
  unsigned x = __VERIFIER_nondet_uint();
  if (x * x == 2)
    for (unsigned i = 0; i < 1000000000; ++i)
      x += __VERIFIER_nondet_uint();
}
