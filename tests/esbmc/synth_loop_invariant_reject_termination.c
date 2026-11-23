// --termination havocs every loop head k-induction-style, and the invariant
// schema has already rewritten those heads: the establishment ASSERT sits where
// havoc_slot expects the guard. Reject the combination rather than abort.
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
// Imported from ESBMC regression/esbmc/synth_loop_invariant_reject_termination
// Needs: --synthesise-loop-invariants --termination
// RUN: --synthesise-loop-invariants --termination
// MODE: FUTURE
// EXPECT: ^ERROR: --synthesise-loop-invariants cannot be combined with --termination$
// EXPECT: \A(?![\s\S]*VERIFICATION)[\s\S]*\Z
// Vereda: verdict none
