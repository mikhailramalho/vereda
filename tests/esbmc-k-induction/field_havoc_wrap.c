#include <assert.h>
#include <stdint.h>

typedef struct
{
  unsigned cfg;
  unsigned hdr[8];
} state_t;

static state_t state;
unsigned __VERIFIER_nondet_uint(void);

int main(void)
{
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n <= 1000000u);
  state.cfg = 0;
  unsigned *dest = &state.hdr[0];
  for (unsigned i = 0; i < n; ++i)
  {
    if (i == 0)
    {
      dest[0x3FFFFFFFFFFFFFFFull] = 1;
    }
    if (i == 100)
      assert(state.cfg == 0);
  }
}
// Imported from ESBMC regression/k-induction/field_havoc_wrap
// Needs: --k-induction
// RUN: --k-induction --k-step 4 --max-k-step 5 --ub-pointer-check=false --no-bounds-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none
