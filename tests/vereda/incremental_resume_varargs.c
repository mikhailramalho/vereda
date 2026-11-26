// The callee's vararg context survives a cut before va_start, which executes
// only after its loop has completed at a larger bound.
// RUN: --no-summarize-array-loops --incremental-bmc --max-k-step 10
// RUN: --no-summarize-array-loops --incremental-bmc --max-k-step 10 -m32
// EXPECT: \[PROGRESS\] Resuming main
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdarg.h>
extern void __VERIFIER_assert(int);

static int sum(int n, ...) {
  int count = 0;
  for (int i = 0; i < 6; ++i)
    ++count;
  va_list args;
  va_start(args, n);
  for (int i = 0; i < n; ++i)
    count += va_arg(args, int);
  va_end(args);
  return count;
}

int main(void) {
  __VERIFIER_assert(sum(2, 7, 8) == 21);
}
