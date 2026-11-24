/* Boundary for #5393: the same nondet pointer survives when the allocation
   carries a type, because the store is then a whole-element write rather than
   a byte-level one. malloc(sizeof(void *)) types the object; calloc, and any
   malloc whose size is a runtime value, do not. */
#include <stdlib.h>

void *__VERIFIER_nondet_voidp(void);

int main(void)
{
  void *q = __VERIFIER_nondet_voidp();
  __VERIFIER_assume(q != 0);

  void **s = malloc(sizeof(void *));
  if (s == 0)
    return 0;

  *s = q;

  __VERIFIER_assert(*s != 0); // a typed allocation keeps the pointer
  return 0;
}
// Imported from ESBMC regression/esbmc/github_5393_nondet_ptr_typed_alloc
// RUN: --no-pointer-check
// EXPECT: ^VERIFICATION SUCCESSFUL$
