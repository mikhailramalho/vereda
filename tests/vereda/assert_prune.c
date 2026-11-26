// RUN: --unwind 2
// EXPECT: ^VERIFICATION FAILED$
// EXPECT-NOT: unwinding assertion
// An unconditional predicate failure ends the single-property path.
extern void __VERIFIER_assert(int);
int main(void) {
  __VERIFIER_assert(0);
  for (unsigned i = 0; i < 1000; ++i)
    __VERIFIER_assert(i < 1000);
}
