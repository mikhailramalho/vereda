// The first iteration changes only the rounding mode, which ESBMC keeps in an
// internal variable. The state that decides the loop's future includes it, so
// the second visit to the head is no repeat.
#include <fenv.h>

int main(void)
{
  int c = 0;
  while (1)
  {
    if (fegetround() == FE_TONEAREST)
    {
      fesetround(FE_DOWNWARD);
      continue;
    }
    c++;
    if (c > 3)
      break;
  }
  return 0;
}
// Imported from ESBMC regression/termination/recurrent_state_hidden_state
// Needs: --termination --k-step 2 --max-inductive-step 3 --no-assertions
// RUN: --termination --k-step 2 --max-k-step 12 --max-inductive-step 3 --no-pointer-check --no-bounds-check --no-assertions
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
