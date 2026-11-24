// esbmc/esbmc#7855: a pointer stored over and over is recorded once per store;
// those records must not push an earlier pointer past the rebuild cap.
#include <stdlib.h>

int main(void)
{
  unsigned long n = __VERIFIER_nondet_ulong();
  __VERIFIER_assume(n >= 2 && n <= 3);
  const void **slots = malloc(n * sizeof(void *));
  if (!slots)
    return 0;

  unsigned long i = __VERIFIER_nondet_ulong(), j = __VERIFIER_nondet_ulong();
  __VERIFIER_assume(i < n && j < n && i != j);
  const void *key, *other;
  slots[i] = key;
  for (int k = 0; k < 40; ++k)
    slots[j] = other;

  __VERIFIER_assert(slots[i] == key); // the stored key reads back
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7855-repeated-store
// RUN: -m64 --incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
