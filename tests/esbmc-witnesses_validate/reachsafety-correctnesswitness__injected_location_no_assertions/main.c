#include <limits.h>
extern int __VERIFIER_nondet_int(void);
int main(void)
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x >= 0 && x < INT_MAX);
  x += 1;
  __VERIFIER_assert(0); // ignored user assertion
  return 0;
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-correctnesswitness/injected_location_no_assertions
// Needs: --no-assertions --validate-correctness-witness --witness witness.yml
// RUN: --no-assertions --overflow-check --no-pointer-check --no-bounds-check --validate-correctness-witness --witness witness.yml
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: PASSED.*witness invariant$
// Vereda: verdict none
