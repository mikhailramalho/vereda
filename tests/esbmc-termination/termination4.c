#include <assert.h>

int main()
{
  int i = 10;
  while(i>=0) i--;
  return 0;
}
// Imported from ESBMC regression/termination/termination4
// Needs: --state-hashing --add-symex-value-sets --no-assertions --termination --max-inductive-step 3
// RUN: --no-div-by-zero-check --force-malloc-success --state-hashing --add-symex-value-sets --no-align-check --k-step 2  --unlimited-k-steps --no-vla-size-check --64 --no-pointer-check --no-bounds-check --no-assertions --termination --max-inductive-step 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
