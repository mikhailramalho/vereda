// A recursive continuation restores each caller's n before using it in a return.
// RUN: --incremental-bmc --max-k-step 10
// RUN: --incremental-bmc --max-k-step 10 --incremental-solver
// EXPECT: \[PROGRESS\] Resuming main
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

static unsigned sum(unsigned n) {
  if (n == 0)
    return 0;
  return n + sum(n - 1);
}

int main(void) {
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n < 8);
  unsigned result = sum(n);
  __VERIFIER_assert(result == n * (n + 1) / 2);
}
