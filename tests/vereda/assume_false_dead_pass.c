// A folded __VERIFIER_assume(0) ends the block early, so subsequent dead code
// in the same block is not walked.
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT-NOT: VERIFICATION FAILED
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);

int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x > 0) {
    __VERIFIER_assume(0);
    __VERIFIER_assert(0);
    return 1;
  }
  __VERIFIER_assert(x <= 0);
  return 0;
}
