/* github #9228: alignment constrains only as many low bits as it names, so a
 * 32-byte-aligned object need not be 64-byte aligned. */
#include <assert.h>
#include <stdint.h>

_Alignas(32) char buf[3];

int main(void)
{
  assert((uintptr_t)buf % 64 == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9228_alignment_fail
// Needs: --std c11
// RUN: --std c11 --64
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
