// process_goto_program routes --loop-invariant into goto_loop_invariant_combined,
// which never reaches the synthesis pass, so --synthesise-loop-invariants would
// be a silent no-op. Combined mode also ASSUMEs the invariant at the end of the
// body, and a synthesised guess must never be assumed. Reject the combination.
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
// Imported from ESBMC regression/esbmc/synth_loop_invariant_reject_combined
// Needs: --synthesise-loop-invariants --loop-invariant
// RUN: --synthesise-loop-invariants --loop-invariant --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^ERROR: --synthesise-loop-invariants cannot be combined with --loop-invariant; use --loop-invariant-check, which it implies$
// Vereda: verdict none
