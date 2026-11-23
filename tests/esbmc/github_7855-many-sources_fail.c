// esbmc/esbmc#7855: a slot other than the one the key went into need not
// hold it.
#include <stdlib.h>

const void *__VERIFIER_nondet_ptr(void);

int main(void)
{
  unsigned long n = __VERIFIER_nondet_ulong();
  __VERIFIER_assume(n >= 42 && n <= 44);
  const void **slots = malloc(n * sizeof(void *));
  if (!slots)
    return 0;

  for (int k = 0; k < 40; ++k)
    slots[k] = __VERIFIER_nondet_ptr();

  const void *key;
  unsigned long i = __VERIFIER_nondet_ulong();
  __VERIFIER_assume(i >= 40 && i < n);
  slots[i] = key;

  __VERIFIER_assert(slots[0] == key); // another slot holds the key
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7855-many-sources_fail
// RUN: -m64 --incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
