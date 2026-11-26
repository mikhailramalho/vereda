#include <assert.h>

int main()
{
  while(1);
  return 0;
}
// Imported from ESBMC regression/termination/termination8
// Needs: --no-assertions --termination --max-inductive-step 3
// RUN: --no-div-by-zero-check --force-malloc-success --no-align-check --k-step 2 --no-vla-size-check -m64 --no-pointer-check --no-bounds-check --no-assertions --termination --max-inductive-step 3
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
