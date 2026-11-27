#include <assert.h>

/* The counterexample decodes a union's short member from its lowest address,
   as the verdict does. */
union U
{
  unsigned i;
  unsigned short s;
};

union U __VERIFIER_nondet_U();

int main()
{
  union U h = __VERIFIER_nondet_U();
  __VERIFIER_assume(h.i == 0x01020304u);
  assert(h.s != 0x0102);
  return 0;
}
// Imported from ESBMC regression/esbmc/big_endian_union_trace_fail
// Needs: --big-endian
// RUN: --big-endian --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  h = \{ \.i=16909060, \.s=258 \}$
// Vereda: verdict none
