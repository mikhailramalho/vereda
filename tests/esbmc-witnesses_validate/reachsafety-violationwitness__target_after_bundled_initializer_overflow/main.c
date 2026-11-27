#include <math.h>
int main(void)
{
  int x = __INT_MAX__;
  return x + 1;
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-violationwitness/target_after_bundled_initializer_overflow
// Needs: --no-fp-overflow-check --validate-violation-witness --witness witness.yml
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --no-fp-overflow-check --validate-violation-witness --witness witness.yml --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
