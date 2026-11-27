#include <assert.h>
#include <stdint.h>

int main()
{
  uint32_t d = __VERIFIER_nondet_uint();
  uint16_t c = __VERIFIER_nondet_ushort();
  uint16_t e = __VERIFIER_nondet_ushort();

  // Mixed-width additions – must not simplify
  // (uint32)d + (uint16)c == (uint32)d + (uint16)e  DOES NOT imply c == e  
  // because c and e are zero-extended to 32 bits

  assert(((d + c) == (d + e)) ? (c == e) : 1); // Should NOT guarantee c == e

  return 0;
}

// Imported from ESBMC regression/esbmc/simplifier14
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
