// Join callee returns and restore caller memory, facts and sibling arrivals.
// RUN: --no-summarize-array-loops --incremental-bmc --max-k-step 10
// RUN: --no-summarize-array-loops --incremental-bmc --max-k-step 10 --ub-uninit-check -m32
// EXPECT: \[PROGRESS\] Resuming main
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

static unsigned count(unsigned n, unsigned *p) {
  for (unsigned i = 0; i < n; ++i) {
    ++*p;
    if (i == 3)
      return i + 1;
  }
  return n;
}

int main(void) {
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n < 8);
  unsigned x = 0;
  if (__VERIFIER_nondet_uint()) {
    unsigned result = count(n, &x);
    __VERIFIER_assert(result == x);
    __VERIFIER_assert(result == (n < 4 ? n : 4));
  } else {
    __VERIFIER_assert(x == 0);
  }
}
