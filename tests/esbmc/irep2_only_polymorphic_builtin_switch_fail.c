#include <stdatomic.h>

/* A switch selector is a call site too: the sideeffect2t holding the call
 * carries no location, so the counterexample takes the switch statement's. */
int main(void)
{
  atomic_int *p = 0;
  switch (atomic_load(p))
  {
  case 1:
    return 1;
  default:
    return 0;
  }
}
// Imported from ESBMC regression/esbmc/irep2_only_polymorphic_builtin_switch_fail
// Needs: --clang-c-irep2-adjust-only
// RUN: --clang-c-irep2-adjust-only --std c11
// MODE: FUTURE
// EXPECT: ^\s*FAILED\s+\[__c11_atomic_load\.null-pointer-dereference\.1\]\s+line 8\s+dereference failure: NULL pointer$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
