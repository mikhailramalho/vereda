/* glibc spells assert(e) with a leading `(void) sizeof ((e) ? 1 : 0)`, which
 * lowers to an OTHER instruction in the loop body. The body scan whitelists
 * instruction kinds and refused every OTHER, so the synthesiser declined every
 * asserting loop on Linux -- which is what failed synth_loop_invariant_lowerbnd
 * in CI. The expansion is written out here so the shape is pinned on every
 * host, not only on Linux. */
#include <stdint.h>
extern void
__assert_fail(const char *, const char *, unsigned int, const char *);
#define ASSERT_GLIBC(expr)                                                     \
  ((void)sizeof((expr) ? 1 : 0), __extension__({                               \
     if (expr)                                                                 \
       ;                                                                       \
     else                                                                      \
       __assert_fail(#expr, __FILE__, __LINE__, __func__);                     \
   }))
int main(void)
{
  uint32_t n;
  uint64_t a;
  uint64_t i = 1, sn = 0;
  __VERIFIER_assume(n >= 1 && n <= 10);
  __VERIFIER_assume(a <= 10);
  while (i <= n)
  {
    ASSERT_GLIBC(sn <= (uint64_t)n * a);
    sn = sn + a;
    i++;
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/synth_loop_invariant_glibcassert
// Needs: --synthesise-loop-invariants
// RUN: --synthesise-loop-invariants --unwind 12
// MODE: FUTURE
// EXPECT: ^Synthesised loop invariants for 1 loop$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
