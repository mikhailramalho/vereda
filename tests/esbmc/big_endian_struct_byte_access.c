#include <assert.h>

/* Byte k of a struct of 16-bit members, at a nondet k. */
struct P
{
  unsigned short x, y;
};

int main()
{
  struct P p;
  p.x = __VERIFIER_nondet_ushort();
  p.y = __VERIFIER_nondet_ushort();
  unsigned k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(k < 4);
  unsigned char *c = (unsigned char *)&p;
  unsigned short v = k < 2 ? p.x : p.y;
  assert(c[k] == (unsigned char)(k % 2 == 0 ? v >> 8 : v));
  return 0;
}
// Imported from ESBMC regression/esbmc/big_endian_struct_byte_access
// Needs: --big-endian
// RUN: --big-endian --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
