#include <math.h>
int main(void)
{
  return 0;
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-violationwitness/target_after_bundled_initializer
// Needs: --no-fp-overflow-check --validate-violation-witness --witness witness.yml
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --no-fp-overflow-check --validate-violation-witness --witness witness.yml --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
