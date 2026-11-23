/* Arrays, structs and floats must all reach the solver flattened to pure
 * QF_BV: the stand-in runs a real z3 on the emitted formula, so any leaked
 * Array or FloatingPoint sort fails the (set-logic QF_BV) run loudly. */
struct point
{
  int x;
  float y;
};

int main()
{
  int a[4];
  struct point p;
  unsigned i = __VERIFIER_nondet_uint() % 4;
  a[i] = 42;
  p.x = a[i];
  p.y = 1.5f;
  __VERIFIER_assert(p.x == 42 && p.y == 1.5f); // flattened features round-trip
  return 0;
}
// Imported from ESBMC regression/neurosym/flatten-safe
// Needs: --neurosym --neurosym-prog fake-neurosym.sh
// RUN: --neurosym --neurosym-prog fake-neurosym.sh
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
