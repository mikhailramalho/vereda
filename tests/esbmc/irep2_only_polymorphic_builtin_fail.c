#include <stdatomic.h>

/* The instantiated body carries the memory-safety obligations of the access
 * the builtin performs. Body-less, the store is a no-op and the null
 * dereference is missed entirely. */
int main(void)
{
  atomic_int *p = 0;
  atomic_store(p, 1);
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_polymorphic_builtin_fail
// Needs: --clang-c-irep2-adjust-only
// RUN: --clang-c-irep2-adjust-only --std c11
// MODE: FUTURE
// EXPECT: ^\s*FAILED\s+\[__c11_atomic_store\.null-pointer-dereference\.1\]\s+line 9\s+dereference failure: NULL pointer$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
