// An unknown trip count still needs an unwinding query: an infeasible
// cut can establish safety without a known bound to advance toward.
// RUN: --no-summarize-array-loops --max-k-step 4
// EXPECT-NOT: Deferring unwinding-only solve
// EXPECT-NOT: Checking bound k = 2$
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern unsigned __VERIFIER_nondet_uint(void);
int main(void) {
  unsigned x = __VERIFIER_nondet_uint();
  if (x * x == 2)
    while (__VERIFIER_nondet_uint()) {}
}
