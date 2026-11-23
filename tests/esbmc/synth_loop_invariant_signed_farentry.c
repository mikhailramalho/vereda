/* Entry value far past any admissible bound, so the loop never runs. The
 * accumulator must still read as its entry value at the exit. */
int __VERIFIER_nondet_int();
int main(void)
{
  int n = __VERIFIER_nondet_int();
  __VERIFIER_assume(n >= 0 && n <= 20);
  int i = 1000, s = 0;
  while (i < n)
  {
    s = s + 1;
    i++;
  }
  __VERIFIER_assert(s == 0); // loop never entered
  return 0;
}
// Imported from ESBMC regression/esbmc/synth_loop_invariant_signed_farentry
// Needs: --synthesise-loop-invariants
// RUN: --synthesise-loop-invariants
// MODE: FUTURE
// EXPECT: ^Synthesised loop invariants for 1 loop$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
