/* goto_k_induction replaces the loop head with its havoc block before the
 * recogniser runs, so --synthesise-loop-invariants can never fire under
 * --k-induction. The run must say so rather than look like one where no loop
 * happened to be affine. */
int main(void)
{
  unsigned int i = 0, s = 0;
  while (i < 4)
  {
    s = s + 2;
    i = i + 1;
  }
  return s;
}
// Imported from ESBMC regression/esbmc/synth_loop_invariant_kinduction
// Needs: --synthesise-loop-invariants --k-induction
// RUN: --synthesise-loop-invariants --k-induction
// MODE: FUTURE
// EXPECT: ^WARNING: --synthesise-loop-invariants added no invariant: the k-induction transform has already rewritten the loop heads it matches on\. Drop the k-induction phase to use it$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
