#include <assert.h>
#include <stddef.h>

/* The smaller of the two objects a pointer may address keeps its own size. */
_Bool __VERIFIER_nondet_bool(void);

int main(void)
{
  char small[1];
  char big[100];
  char *p = __VERIFIER_nondet_bool() ? small : big + 50;
  assert(__CPROVER_OBJECT_SIZE(p) == 100);
  return 0;
}
// Imported from ESBMC regression/esbmc/cprover_object_size_per_object_fail
// EXPECT: ^VERIFICATION FAILED$
