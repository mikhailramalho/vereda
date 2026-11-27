#include <assert.h>

int main()
{
  while(1);
  return 0;
}
// Imported from ESBMC regression/termination/termination8
// Needs: --termination --max-inductive-step 3
// RUN: --ub-div-by-zero-check=false --force-malloc-success --k-step 2 --vla-size-check=false -m64 --ub-pointer-check=false --no-assertions --termination --max-inductive-step 3
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
