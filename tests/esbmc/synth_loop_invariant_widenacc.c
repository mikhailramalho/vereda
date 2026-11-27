/* The sum01 shape with the accumulator widened to long. The counter
 * difference is taken at the accumulator's type: computing it at the
 * counter's and widening afterwards let the narrow subtraction wrap where the
 * widening did not, so the havoc could pick i near INT_MIN and fail the
 * inductive step on a correct program. */
int __VERIFIER_nondet_int(void);

int main(void)
{
  int n = __VERIFIER_nondet_int();
  __VERIFIER_assume(n >= 1 && n <= 8);

  int i = 1;
  long sn = 0;

  for (i = 1; i <= n; i++)
    sn = sn + 2;

  __VERIFIER_assert(sn == 2L * n || sn == 0); // sum01-long
  return 0;
}
// Imported from ESBMC regression/esbmc/synth_loop_invariant_widenacc
// Needs: --synthesise-loop-invariants
// RUN: --synthesise-loop-invariants --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^Synthesised loop invariants for 1 loop$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
