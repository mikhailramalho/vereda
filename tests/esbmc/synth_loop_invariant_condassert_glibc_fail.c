/* The counterpart of synth_loop_invariant_condassert_glibc: the same region,
 * with a claim the closed form does not imply. Recognising the glibc spelling
 * must not make the abstraction prove it. */
#include <stdint.h>

extern void
__assert_fail(const char *, const char *, unsigned int, const char *);

int main(void)
{
  uint32_t n;
  uint64_t a;
  uint64_t i = 1, sn = 0;
  __VERIFIER_assume(n >= 1 && n <= 10);
  __VERIFIER_assume(a <= 10);
  while (i <= n)
  {
    if (i > 1)
      ((void)sizeof((sn > a) ? 1 : 0), __extension__({
         if (sn > a)
           ;
         else
           __assert_fail("sn > a", "main.c", 22, __func__);
       }));
    sn = sn + a;
    i++;
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/synth_loop_invariant_condassert_glibc_fail
// Needs: --synthesise-loop-invariants
// RUN: --synthesise-loop-invariants --unwind 12
// MODE: FUTURE
// EXPECT: ^Synthesised loop invariants for 1 loop$
// EXPECT: loop invariant too weak to prove this claim
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none
