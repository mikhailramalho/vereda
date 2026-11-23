#include <string.h>

int __VERIFIER_nondet_int();
unsigned __VERIFIER_nondet_uint();

/* An integer-derived address names no object. The builtins resolved a pointer
 * that might also hold one to that object alone and dropped the check. */
int main()
{
  char a[4] = {1, 2, 3, 4}, b[4] = {1, 2, 3, 4};
  char *p = (char *)0x1000;
  if (__VERIFIER_nondet_int())
    p = a;

  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n > 0 && n <= 4);

  int r = memcmp(p, b, 4);
  memset(p, 0, 4);
  char *c = memchr(p, 3, 4);
  memcpy(b, p, 4);
  memcpy(p, b, n);
  return r + (c != 0);
}
// Imported from ESBMC regression/esbmc/mem_intrinsics_invalid_pointer_fail
// RUN: --multi-property
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: FAILED.*dereference failure: invalid pointer on memcmp$
// EXPECT: FAILED.*dereference failure: invalid pointer on memset$
// EXPECT: FAILED.*dereference failure: invalid pointer on memchr$
// EXPECT: FAILED.*dereference failure: invalid pointer on SRC$
// EXPECT: FAILED.*dereference failure: invalid pointer on DST$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
