#include <stdint.h>
typedef struct
{
  uint32_t cfg;
  uint32_t shadow[8];
  uint32_t tail;
} state_t;
static state_t g;
int main(void)
{
  unsigned int k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(k < 2);
  uint32_t *p = g.shadow + k;
  p[7] = 5;
  return 0;
}
// Imported from ESBMC regression/esbmc/array_member_decay_overflow_fail
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: FAILED +\[main\.array-bounds-violated\.1\]
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
