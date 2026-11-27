#include <stdint.h>
#include <stdlib.h>

unsigned long __VERIFIER_nondet_ulong(void);
char a[64], b[64];

/* Freeing a global through a pointer the value set cannot resolve. */
int main(void)
{
  __VERIFIER_assume((uintptr_t)&b[0] != 0);
  unsigned long u = __VERIFIER_nondet_ulong();
  char *p = (char *)(u * 8 - (uintptr_t)&a[0]);
  if ((uintptr_t)p == (uintptr_t)&b[0])
    free(p);
  return 0;
}
// Imported from ESBMC regression/esbmc/deref_unmodelled_free
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^.*dereference failure: invalid pointer freed$
// Vereda: unmet EXPECT: ^.*dereference failure: invalid pointer freed$
// MODE: FUTURE
// Vereda: verdict FAILED
