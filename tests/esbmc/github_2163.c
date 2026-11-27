// example.c

/* Two departures from the issue's verbatim reproducer, both for portability:
 *
 * - It read uninitialised locals, which is undefined behaviour; nondet_int()
 *   states the "arbitrary value" intent directly.
 * - It used assert(). MSVC's assert.h puts the failure path under !(cond), so
 *   with `__VERIFIER_assume(c < 100)` right above it that path is unsatisfiable
 *   and coverage correctly reports the assertion UNREACHED -- a different
 *   answer from the same source on Linux/macOS, where assert() lowers to an
 *   unconditional check. __VERIFIER_assert is unconditional on every target.
 */

int main()
{
  int x = __VERIFIER_nondet_int();
  int y = __VERIFIER_nondet_int();

  __VERIFIER_assume(x < 100);
  __VERIFIER_assume(x > 0);

  __VERIFIER_assume(y < 100);
  __VERIFIER_assume(y > 0);

  int c = x + y;

  __VERIFIER_assume(c < 100);
  __VERIFIER_assert(c < 100); // c < 100

  return c;
}
// Imported from ESBMC regression/esbmc/github_2163
// Needs: --assertion-coverage-claims
// RUN: --assertion-coverage-claims --no-incremental-bmc
// MODE: FUTURE
// EXPECT: Assertion Instances Coverage: 100%
// EXPECT: : REACHED$
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
