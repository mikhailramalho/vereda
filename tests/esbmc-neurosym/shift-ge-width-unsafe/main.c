/* SMT-LIB2 FixedSizeBitVectors folds a shift by >= the operand width to
 * zero. The shift amount here reaches the backend's local model evaluator
 * straight from NeuroSym's own model output, where evaluating it as a plain
 * BigInt shift instead builds 2^y through a y-iteration multiply loop and
 * does not terminate. Wrapping the result in a struct is what makes the
 * trace query the shift as a composite expression rather than a symbol. */
struct wrap
{
  unsigned int f;
};

int main()
{
  unsigned int x = __VERIFIER_nondet_uint();
  unsigned int y = __VERIFIER_nondet_uint();
  struct wrap w;

  /* Pinned, not merely >= 32: an unguarded evaluator computes 2^y through
   * a y-iteration BigInt multiply loop, so only a shift amount this large
   * makes the missing guard exceed the test timeout. A solver left free to
   * pick any y >= 32 picks a small one, and the test stops biting. */
  __VERIFIER_assume(y == 100000000u);
  w.f = x << y;
  __VERIFIER_assert(w.f != 0u); // shift by at least the width is non-zero
  return 0;
}
// Imported from ESBMC regression/neurosym/shift-ge-width-unsafe
// Needs: --neurosym --neurosym-prog fake-neurosym.sh --neurosym-model-prog z3 -in
// RUN: --neurosym --neurosym-prog fake-neurosym.sh --neurosym-model-prog "z3 -in"
// MODE: FUTURE
// EXPECT: ^\[COUNTEREXAMPLE\]$
// EXPECT: ^  w\.f = 0; // 00000000 00000000 00000000 00000000$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
