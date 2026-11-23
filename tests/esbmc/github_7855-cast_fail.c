// esbmc/esbmc#7855: perturbing the address must still break the round trip.
// Pairs with github_7855-cast, which a fix that makes an integer-to-pointer
// cast unconditionally reproduce its operand's pointer would turn green
// without this one going red.
#include <stdint.h>

int main(void)
{
  const void *key;
  uintptr_t address = (uintptr_t)key;
  const void *back = (const void *)(address + 1);

  __VERIFIER_assert(back == key); // a different address is a different pointer
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7855-cast_fail
// RUN: -m64 --incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
