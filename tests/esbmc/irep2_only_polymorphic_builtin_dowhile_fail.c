#include <stdatomic.h>

/* The call is a sideeffect2t in a do-while condition. Before sideeffect2t
 * carried a location, the enclosing statement's was used and this reported
 * line 9, the `do`; the default path reports line 11, the call. §136. */
int main(void)
{
  atomic_int *p = 0;
  do
    ;
  while (atomic_load(p) < 10);
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_polymorphic_builtin_dowhile_fail
// Needs: --clang-c-irep2-adjust-only
// RUN: --clang-c-irep2-adjust-only --std c11 --incremental-bmc
// MODE: FUTURE
// EXPECT: ^\s*FAILED\s+\[__c11_atomic_load\.null-pointer-dereference\.1\]\s+line 11\s+dereference failure: NULL pointer$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
