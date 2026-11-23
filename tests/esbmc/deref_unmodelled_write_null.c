#include <stdint.h>

_Bool __VERIFIER_nondet_bool(void);
unsigned long __VERIFIER_nondet_ulong(void);
char a[64];

/* A value set holding NULL beside unknown: NULL keeps its own claim, and the
 * unresolved branch is reported separately. */
int main(void)
{
  unsigned long u = __VERIFIER_nondet_ulong();
  char *p = __VERIFIER_nondet_bool() ? (char *)0 : (char *)(u * 8 - (uintptr_t)&a[0]);
  *p = 3;
  return 0;
}
// Imported from ESBMC regression/esbmc/deref_unmodelled_write_null
// RUN: --multi-property
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^.*FAILED.*dereference failure: NULL pointer$
// EXPECT: ^.*FAILED.*dereference failure: invalid pointer$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
