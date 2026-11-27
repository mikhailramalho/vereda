#include <string.h>

int __VERIFIER_nondet_int();

int main()
{
  char a[4] = {0}, b[4] = {0};
  char *p = a;
  if (__VERIFIER_nondet_int())
    p = 0;
  return memcmp(p, b, 4);
}
// Imported from ESBMC regression/esbmc/memcmp_null_operand_fail
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  dereference failure: NULL pointer on memcmp$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
