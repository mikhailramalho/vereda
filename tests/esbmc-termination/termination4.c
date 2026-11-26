#include <assert.h>

int main()
{
  int i = 10;
  while(i>=0) i--;
  return 0;
}
// Imported from ESBMC regression/termination/termination4
// Needs: --no-assertions --termination --max-inductive-step 3
// RUN: --no-div-by-zero-check --force-malloc-success --no-align-check --k-step 2 --unlimited-k-steps --no-vla-size-check -m64 --no-pointer-check --no-bounds-check --no-assertions --termination --max-inductive-step 3
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
