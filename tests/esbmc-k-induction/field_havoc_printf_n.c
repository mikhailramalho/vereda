#include <assert.h>
#include <stdio.h>
struct state_t
{
  int cfg;
  unsigned hdr[8];
};
static struct state_t state;
unsigned __VERIFIER_nondet_uint(void);
static void fill(unsigned *dest)
{
  *dest = 0;
}
int main(void)
{
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n <= 1000);
  state.cfg = 0;
  for (unsigned i = 0; i < n; ++i)
  {
    fill(&state.hdr[0]);
    if (i == 0)
      printf("x%n", &state.cfg);
    if (i == 100)
      assert(state.cfg == 0);
  }
}
// Imported from ESBMC regression/k-induction/field_havoc_printf_n
// Needs: --k-induction
// RUN: --k-induction --k-step 4 --max-k-step 5
// MODE: FUTURE
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none
