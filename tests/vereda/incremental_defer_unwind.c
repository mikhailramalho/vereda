// Extending a finite loop needs no unwinding-only query before its known
// bound, but the assertion reached after it must still be checked.
// RUN: --no-summarize-array-loops
// RUN: --no-summarize-array-loops --incremental-solver
// RUN: --no-summarize-array-loops --fresh-solver
// RUN: --no-summarize-array-loops --max-k-step 5 --k-step 2
// EXPECT: Deferring unwinding-only solve
// EXPECT: Resuming main
// EXPECT: ^VERIFICATION FAILED$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assert(int);
int main(void) {
  unsigned x = __VERIFIER_nondet_uint();
  if (x == 7) {
    for (unsigned i = 0; i < 3; ++i)
      x += __VERIFIER_nondet_uint();
    __VERIFIER_assert(x != 42);
  }
}
