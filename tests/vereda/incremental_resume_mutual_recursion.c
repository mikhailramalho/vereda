// Restore bindings on each side of mutual recursion and across sibling calls.
// RUN: --incremental-bmc --max-k-step 10
// EXPECT: \[PROGRESS\] Resuming main
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
static unsigned even(unsigned);

static unsigned odd(unsigned n) {
  return n ? n + even(n - 1) : 0;
}

static unsigned even(unsigned n) {
  return n ? n + odd(n - 1) : 0;
}

int main(void) {
  unsigned a = even(7);
  unsigned b = odd(6);
  __VERIFIER_assert(a == 28 && b == 21);
}
