#include <stdatomic.h>

/* The call is a sideeffect2t in a loop condition, which carries no location of
 * its own. Unless the enclosing statement's is picked up, the counterexample
 * names no file and reports line 0. */
int main(void)
{
  atomic_int *p = 0;
  while (atomic_load(p) < 10)
    ;
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_polymorphic_builtin_cond_fail
// Needs: --clang-c-irep2-adjust-only --std c11
// RUN: --clang-c-irep2-adjust-only --std c11 --incremental-bmc
// MODE: FUTURE
// EXPECT: ^\s*FAILED\s+\[__c11_atomic_load\.null-pointer-dereference\.1\]\s+line 9\s+dereference failure: NULL pointer$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
