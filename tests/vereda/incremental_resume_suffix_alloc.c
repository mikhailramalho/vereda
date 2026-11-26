// When the post-loop suffix allocates heap memory (advancing global layout
// constraints), the checkpoint is discarded so the next bound starts fresh.
// RUN: --no-summarize-array-loops --incremental-bmc --max-k-step 8 --force-malloc-success
// EXPECT-NOT: \[PROGRESS\] Resuming
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdlib.h>

extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

int main(void) {
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n < 5);
  unsigned count = 0;
  while (count < n)
    ++count;
  int *p = malloc(sizeof(int));
  *p = (int)count;
  __VERIFIER_assert(*p == (int)n);
  free(p);
  return 0;
}
