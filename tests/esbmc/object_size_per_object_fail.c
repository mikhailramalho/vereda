#include <assert.h>
#include <stddef.h>

/* The smaller of the two objects a pointer may address keeps its own size. */
_Bool __VERIFIER_nondet_bool(void);
size_t __builtin_object_size(const void *, int);

int main(void)
{
  char small[1];
  char big[100];
  char *p = __VERIFIER_nondet_bool() ? small : big + 50;
  assert(__builtin_object_size(p, 0) == 100);
  return 0;
}
// Imported from ESBMC regression/esbmc/object_size_per_object_fail
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
