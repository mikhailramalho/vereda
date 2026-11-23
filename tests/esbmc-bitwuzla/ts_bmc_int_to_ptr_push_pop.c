#include <stdint.h>
int g[2];
int main(void)
{
  for (uintptr_t u = 0; u < 16; u += sizeof(int))
    __VERIFIER_assert(
      (int *)u != &g[1] || u == (uintptr_t)&g[1]
); // only g[1]'s address names g[1]
  return 0;
}
// Imported from ESBMC regression/bitwuzla/ts_bmc_int_to_ptr_push_pop
// Needs: --ts-bmc --ts-push-pop --ts-no-fallback
// RUN: --bitwuzla --64 --ts-bmc --ts-push-pop --ts-no-fallback --max-k-step 10
// MODE: FUTURE
// EXPECT: ^TS BMC step 10: safe
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none
