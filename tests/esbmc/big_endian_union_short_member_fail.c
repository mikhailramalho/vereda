#include <assert.h>

/* The little-endian reading of big_endian_union_short_member. */
union U
{
  unsigned i;
  unsigned short s;
};

int main()
{
  union U u;
  u.i = 0x01020304u;
  assert(u.s == 0x0304);
  return 0;
}
// Imported from ESBMC regression/esbmc/big_endian_union_short_member_fail
// Needs: --big-endian
// RUN: --big-endian
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
