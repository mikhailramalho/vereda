/* The unrolled model covers only the bytes the objects actually have, so a
   length that could exceed them must still be reported rather than dropped. */
#include <string.h>

int main()
{
  char src[8];
  char dst[4];

  unsigned long n = __VERIFIER_nondet_ulong();
  __VERIFIER_assume(n <= 8);

  memcpy(dst, src, n); /* n may exceed dst's 4 bytes */
  return 0;
}
// Imported from ESBMC regression/esbmc/memcpy_symbolic_length_oob_fail
// RUN: --unwind 20
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^ *FAILED .*memcpy length exceeds object bounds$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
