/* Boundary for #5393: a pointer the value set can track survives the same
   untyped-byte-allocation round trip that loses a nondet one. */
#include <stdlib.h>

int main(void)
{
  int x;
  void *q = &x;

  char *raw = malloc(64);
  if (raw == 0)
    return 0;

  void **s = (void **)raw;
  *s = q;

  __VERIFIER_assert(*s == q); // a concrete pointer round-trips through bytes
  return 0;
}
// Imported from ESBMC regression/esbmc/github_5393_concrete_ptr_byte_alloc
// RUN: --ub-pointer-check=false --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
