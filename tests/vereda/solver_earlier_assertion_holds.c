// The first assertion holds and the second fails. Looking for an earlier
// failing assertion proves the first can never fail, so it counts as passed,
// as it does under --incremental-solver, not as not checked.
// EXPECT: ^Properties: 2 checked, 1 passed, 1 failed, 0 not checked$
// EXPECT: ^VERIFICATION FAILED$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assert(int);
int main(void) {
  unsigned x = __VERIFIER_nondet_uint();
  if (x * x > 3)
    __VERIFIER_assert(x != 0);
  __VERIFIER_assert(x != 5);
}
