// Regression test for the calloc overflow false alarm (github #4433 regression).
//
// On 32-bit targets, calloc(n, sizeof(int)) with n = 2^30 wraps
// total_size to 0 via size_t overflow.  Without the fix, malloc(0) was
// called; memset(res, 0, 0) was a no-op; data[i] read nondet from the
// 0-size allocation, causing the zeroed-memory assertion to fire at k=1.
//
// The fix adds __VERIFIER_assume(nmemb <= SIZE_MAX/size) in calloc, pruning
// all overflow paths (consistent with real calloc returning NULL on
// overflow -> NULL deref -> UB/SIGSEGV, which prevents reach_error).
//
// This test uses fixed-size calloc (no overflow) so the assertion is
// provable: the allocated memory is correctly zero-initialised.
#include <stdlib.h>

void reach_error(void) {}
static void verifier_assert(int cond)
{
  if (!cond)
    reach_error();
}

int main(void)
{
  // Use a small, fixed count that fits in 32-bit without overflow.
  // calloc must return zeroed memory; the assertion must hold.
  int *data = (int *)calloc(4, sizeof(int));
  if (!data)
    return 0;
  verifier_assert(data[0] == 0);
  verifier_assert(data[1] == 0);
  verifier_assert(data[2] == 0);
  verifier_assert(data[3] == 0);
  free(data);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_4433_calloc_overflow_norace
// RUN: --ub-div-by-zero-check=false --force-malloc-success --vla-size-check=false -m32 --ub-pointer-check=false
// EXPECT: ^VERIFICATION SUCCESSFUL$
