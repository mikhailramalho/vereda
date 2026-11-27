// Allocation in a resumed call's suffix still invalidates the checkpoint.
// RUN: --no-summarize-array-loops --incremental-bmc --max-k-step 10 --force-malloc-success
// EXPECT-NOT: \[PROGRESS\] Resuming
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdlib.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

static unsigned count(unsigned n) {
  unsigned i = 0;
  while (i < n)
    ++i;
  return i;
}

int main(void) {
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n < 7);
  unsigned result = count(n);
  unsigned *p = malloc(sizeof(*p));
  *p = result;
  __VERIFIER_assert(*p == n);
  free(p);
}
