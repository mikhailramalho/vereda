/* The count bounds the loop but does not excuse a write that is out of bounds
   within it: iteration 2 already leaves the 2-element array. */
#include <stdint.h>

int main(void)
{
  int a[2] = {0};

#pragma unroll 3
  for (uint32_t j = 0; j < 8; j++)
    a[j] = (int)(j + 1);

  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_pragma_unroll_fail
// RUN: --no-unwinding-assertions --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: array bounds violated: array `a' upper bound
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
