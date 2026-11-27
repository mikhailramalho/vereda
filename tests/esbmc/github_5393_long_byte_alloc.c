/* Boundary for #5393: an integer survives the untyped-byte-allocation round
   trip, because its byte stitching is exact. Only pointers are lost. */
#include <stdlib.h>

long __VERIFIER_nondet_long(void);

int main(void)
{
  long v = __VERIFIER_nondet_long();
  __VERIFIER_assume(v != 0);

  char *raw = malloc(64);
  if (raw == 0)
    return 0;

  long *s = (long *)raw;
  *s = v;

  __VERIFIER_assert(*s != 0); // a stored non-zero long reads back non-zero
  return 0;
}
// Imported from ESBMC regression/esbmc/github_5393_long_byte_alloc
// RUN: --ub-pointer-check=false --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
