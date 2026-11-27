#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

/* A pointer that may address either of two objects has the size of the one it
 * addresses, not of whichever its value set lists first. */
_Bool __VERIFIER_nondet_bool(void);
unsigned __VERIFIER_nondet_uint(void);
size_t __builtin_object_size(const void *, int);

int main(void)
{
  char small[1];
  char big[100];
  char *p = __VERIFIER_nondet_bool() ? small : big + 50;
  if (p == big + 50)
  {
    assert(__builtin_object_size(p, 0) == 50);
    assert(__builtin_object_size(p, 3) == 50);
  }
  else
  {
    assert(__builtin_object_size(p, 0) == 1);
    assert(__builtin_object_size(p, 3) == 1);
  }

  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n > 0 && n < 50);
  char *heap = malloc(n);
  char *q = __VERIFIER_nondet_bool() ? heap : big;
  if (heap && q == heap)
    assert(__builtin_object_size(q, 0) == n);
  free(heap);
  return 0;
}
// Imported from ESBMC regression/esbmc/object_size_per_object
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
