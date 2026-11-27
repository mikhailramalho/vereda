// The same discarded-result shape for alloca, which has the enclosing
// function's lifetime and so must not be reported as a leak. Pins the other
// half of github_822_malloc_no_lhs_fail. #822
// Spelt __builtin_alloca because Windows ships no <alloca.h>; goto-convert
// routes both spellings through the same do_mem path.

int main()
{
  __builtin_alloca(10);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_822_alloca_no_lhs
// Needs: --memory-leak-check
// RUN: --memory-leak-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
