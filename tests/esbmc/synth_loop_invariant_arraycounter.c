/* The guard counts on `a[0]`, an index expression rather than a plain symbol.
 * The closed form is built over the counter symbol, so the recogniser
 * declines. */
#include <assert.h>

int main(void)
{
  unsigned int a[2];
  unsigned int n;
  __VERIFIER_assume(n <= 3);

  a[0] = 0;

  while (a[0] < n)
    a[0] = a[0] + 1;

  assert(a[0] == n);
  return 0;
}
// Imported from ESBMC regression/esbmc/synth_loop_invariant_arraycounter
// Needs: --synthesise-loop-invariants
// RUN: --synthesise-loop-invariants --unwind 5
// MODE: FUTURE
// EXPECT: \A(?![\s\S]*Synthesised loop invariants)[\s\S]*\Z
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
