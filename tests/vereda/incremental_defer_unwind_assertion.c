// A live program assertion must be solved before extending a finite loop.
// RUN: --no-summarize-array-loops
// EXPECT: Checking bound k = 1$
// EXPECT-NOT: Checking bound k = 2$
// EXPECT-NOT: Deferring unwinding-only solve
// EXPECT: ^VERIFICATION FAILED$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assert(int);
int main(void) {
  unsigned x = __VERIFIER_nondet_uint();
  if (x == 7)
    for (unsigned i = 0; i < 3; ++i)
      __VERIFIER_assert(__VERIFIER_nondet_uint() != 42);
}
