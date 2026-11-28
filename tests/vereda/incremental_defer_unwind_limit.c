// Do not defer past the configured limit: proving an infeasible cut may
// establish safety even when the finite loop's full bound is unavailable.
// RUN: --no-summarize-array-loops --max-k-step 3
// RUN: --no-summarize-array-loops --max-k-step 4 --k-step 2
// EXPECT: Checking bound k = 3$
// EXPECT-NOT: Deferring unwinding-only solve
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern unsigned __VERIFIER_nondet_uint(void);
int main(void) {
  unsigned x = __VERIFIER_nondet_uint();
  for (unsigned i = 0; i < 2; ++i)
    __VERIFIER_nondet_uint();
  if (x * x == 2)
    for (unsigned i = 0; i < 3; ++i)
      x += __VERIFIER_nondet_uint();
}
