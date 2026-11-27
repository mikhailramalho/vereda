#include <assert.h>

int main()
{
  int i = 10;
  while(i>=0) i--;
  return 0;
}
// Imported from ESBMC regression/termination/termination4
// Needs: --termination --max-inductive-step 3
// RUN: --ub-div-by-zero-check=false --force-malloc-success --k-step 2 --unlimited-k-steps --vla-size-check=false -m64 --ub-pointer-check=false --no-assertions --termination --max-inductive-step 3
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
