// RUN: --multi-property
// EXPECT: FAILED\s+\[main.assertion.1\]
// EXPECT: FAILED\s+\[main.assertion.2\]
// EXPECT: ^VERIFICATION FAILED$
// Independent claims still count after a predicate failure in multi-property mode.
extern void __VERIFIER_assert(int);
int main(void) {
  __VERIFIER_assert(0);
  __VERIFIER_assert(0);
}
