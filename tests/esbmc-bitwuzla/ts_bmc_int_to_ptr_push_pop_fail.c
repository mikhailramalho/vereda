#include <stdint.h>
int g[2];
int main(void)
{
  uintptr_t u = 0;
  while (1)
  {
    __VERIFIER_assert((int *)u != &g[1]); // u never names g[1]
    u = u + sizeof(int);
  }
  return 0;
}
// Imported from ESBMC regression/bitwuzla/ts_bmc_int_to_ptr_push_pop_fail
// Needs: --ts-bmc --ts-push-pop --ts-no-fallback
// RUN: --bitwuzla --64 --ts-bmc --ts-push-pop --ts-no-fallback --max-k-step 6
// MODE: FUTURE
// EXPECT: ^Transition-system counterexample at step 3
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
