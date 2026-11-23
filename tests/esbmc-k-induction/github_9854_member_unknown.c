#include <assert.h>
#include <stdint.h>
unsigned int __VERIFIER_nondet_uint(void);
typedef struct { uint32_t cfg; uint32_t shadow[64]; } state_t;
static state_t g_state;
int main(void)
{
  uint32_t n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n <= 1000000u);
  g_state.cfg = 0u;
  for (uint32_t i = 0; i < n; i++)
  {
    g_state.shadow[i & 63u] = __VERIFIER_nondet_uint();
    assert(g_state.cfg < 100u);
    g_state.cfg++;
  }
  return 0;
}
// Imported from ESBMC regression/k-induction/github_9854_member_unknown
// Needs: --k-induction
// RUN: --k-induction --k-step 4 --max-k-step 9
// MODE: FUTURE
// EXPECT: ^VERIFICATION UNKNOWN$
