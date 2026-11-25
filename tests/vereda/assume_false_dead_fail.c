// A folded __VERIFIER_assume(0) cuts only its own path; a live sibling path
// still reports a failing assertion.
// EXPECT: ^VERIFICATION FAILED$
// EXPECT-NOT: VERIFICATION SUCCESSFUL
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);

int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x > 10) {
    __VERIFIER_assume(0);
    return 1;
  }
  __VERIFIER_assert(x > 0);
  return 0;
}
